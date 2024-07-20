#include <boost/array.hpp>
#include <boost/algorithm/cxx17/reduce.hpp>
#include <boost/algorithm/cxx17/transform_reduce.hpp>
#include <boost/integer/common_factor.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/smart_ptr/make_unique.hpp>
#include <boost/static_assert.hpp>
#include <boost/thread.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

using namespace boost::multiprecision;
using namespace std::chrono_literals;

#define RandomsBufferSize 2048

struct CalculationParams
{
    cpp_int &numCoprimes;
    cpp_int &numIterations;
    const cpp_int &updatePeriod;
    cpp_int minIterations;
    boost::mutex &updatingMutex;
    boost::interprocess::interprocess_semaphore &notifyChanges;
};

struct CalculationThread
{
    cpp_int numCoprimes;
    cpp_int numIterations;
    std::unique_ptr<boost::mutex> mutex;
    std::unique_ptr<boost::thread> thread;
};

void calculateValues(CalculationParams params);
std::ostream &outputPi(std::ostream &output, const cpp_int &numOfCoprimes, const cpp_int &totalNumbers);

int main()
{
    const uint32_t numOfThreads = std::max(1u, boost::thread::hardware_concurrency());
    std::cout << "Working with " << numOfThreads << " threads\n";

    const cpp_int
        minIterations = (cpp_int(1) << 128u),
        updatePeriod = (cpp_int(1) << 24u);

    cpp_int sumNumCoprimes, sumNumIterations;

    std::vector<CalculationThread> threads;

    threads.reserve(numOfThreads);

    boost::interprocess::interprocess_semaphore buffersUpdated(0);

    for (uint i = 0; i < numOfThreads; ++i)
    {
        auto threadMutex = std::make_unique<boost::mutex>();
        threads.emplace_back(CalculationThread{
            .numCoprimes = 0,
            .numIterations = 0,
            .mutex = std::move(threadMutex),
            .thread = nullptr,
        });

        CalculationThread &thread = threads[i];
        thread.thread =
            std::make_unique<boost::thread>(
                calculateValues,
                CalculationParams{
                    .numCoprimes = thread.numCoprimes,
                    .numIterations = thread.numIterations,
                    .updatePeriod = updatePeriod,
                    .minIterations = minIterations,
                    .updatingMutex = *threads[i].mutex,
                    .notifyChanges = buffersUpdated,
                });
    }

    std::cout << "Pi = " << std::setprecision(20) << std::endl;

    while (!threads.empty())
    {
        buffersUpdated.wait();

        for (auto &thread : threads)
        {
            boost::lock_guard lock(*thread.mutex);

            sumNumCoprimes += thread.numCoprimes;
            sumNumIterations += thread.numIterations;

            thread.numCoprimes = 0;
            thread.numIterations = 0;
        }

        outputPi(std::cout, sumNumCoprimes, sumNumIterations) << '\n';

        const auto firstRemove = std::remove_if(threads.begin(), threads.end(),
                                                [](auto &thread)
                                                { return thread.thread->try_join_for(boost::chrono::seconds(1)); });

        threads.erase(firstRemove, threads.end());
    }

    std::cout << "All threads are over" << std::endl;

    std::ofstream file("PI.txt");

    outputPi(
        file << "PI = " << std::setprecision(100),
        sumNumCoprimes,
        sumNumIterations)
        << std::endl;

    file.close();
    std::cout << "Finished storing result into PI.txt" << std::endl;

    return EXIT_SUCCESS;
}

std::ostream &outputPi(std::ostream &output, const cpp_int &numOfCoprimes, const cpp_int &totalNumbers)
{
    return output << boost::multiprecision::sqrt(
               6 /
               ((cpp_dec_float_100)numOfCoprimes /
                (cpp_dec_float_100)totalNumbers));
}

// https://math.stackexchange.com/a/2156132
inline uint32_t binary_gcd(uint32_t u, uint32_t v)
{
    auto shift = __builtin_ctz(u | v);
    u >>= __builtin_ctz(u);
    do
    {
        v >>= __builtin_ctz(v);
        if (u > v)
            std::swap(u, v);
    } while ((v -= u));
    return u << shift;
}

void calculateValues(CalculationParams params)
{
    const uint32_t seed = std::random_device()();
    boost::random::mt11213b gen(seed);
    boost::array<uint32_t, RandomsBufferSize * 2> buffer;
    cpp_int
        generatedCoprimes(0),
        generatedNumbers(0);

    auto &[numCoprimes,
           numIterations,
           updatePeriod,
           minIterations,
           updatingMutex,
           notifyChanges] = params;

    bool done = false;
    while (!done)
    {
        while (generatedNumbers < updatePeriod)
        {
            gen.generate(buffer.begin(), buffer.end());

            generatedCoprimes += boost::algorithm::transform_reduce(
                buffer.begin(),
                buffer.begin() + RandomsBufferSize,
                buffer.begin() + RandomsBufferSize,
                0L,
                std::plus(),
                [](const auto &a, const auto &b)
                {
                    // Both are even numbers, divisible by at least 2.
                    if (!((a | b) & 1))
                        return false;

                    return binary_gcd(a, b) == 1;
                });

            generatedNumbers += RandomsBufferSize;
        }

        {
            boost::lock_guard lock(updatingMutex);

            numCoprimes += generatedCoprimes;
            numIterations += generatedNumbers;

            minIterations -= numIterations;
            done = minIterations <= 0;
        }

        generatedCoprimes = 0;
        generatedNumbers = 0;

        notifyChanges.post();
    }
}
