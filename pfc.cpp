#include <boost/algorithm/cxx17/reduce.hpp>
#include <boost/algorithm/cxx17/transform_reduce.hpp>
#include <boost/integer/common_factor.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/static_assert.hpp>
#include <boost/thread/thread.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

using namespace boost::multiprecision;
using namespace std::chrono_literals;

#define NumOfThreads 5

std::vector<cpp_int> numCoprimesBuffer(NumOfThreads);
std::vector<cpp_int> numIterationsBuffer(NumOfThreads);

struct CalculationParams
{
    cpp_int *const numCoprimes;
    cpp_int *const numIterations;
    const cpp_int &limit;
};

void calculateValues(CalculationParams params);
std::ostream &outputPi(std::ostream &output, const cpp_int &numOfCoprimes, const cpp_int &totalNumbers);

int main()
{
    const cpp_int &limit = (cpp_int(1) << 128);
    std::vector<boost::thread> threads;

    for (int i = 0; i < NumOfThreads; ++i)
    {
        numCoprimesBuffer.emplace_back(0);
        numIterationsBuffer.emplace_back(0);

        threads.emplace_back(
            calculateValues,
            CalculationParams{
                .numCoprimes = &numCoprimesBuffer[i],
                .numIterations = &numIterationsBuffer[i],
                .limit = limit,
            });
    }

    std::cout << "Pi = " << std::setprecision(20) << std::endl;

    cpp_int sumNumCoprimes, sumNumIterations;
    while (!threads.empty())
    {
        const auto firstRemove = std::remove_if(threads.begin(), threads.end(), [](auto &thread)
                                                { return thread.try_join_for(boost::chrono::seconds(1)); });

        threads.erase(firstRemove, threads.end());

        boost::this_thread::sleep_for(boost::chrono::seconds(2));

        sumNumCoprimes = boost::algorithm::reduce(numCoprimesBuffer, cpp_int(0));
        sumNumIterations = boost::algorithm::reduce(numIterationsBuffer, cpp_int(0));

        outputPi(std::cout, sumNumCoprimes, sumNumIterations) << '\n';
    }

    std::cout << "All threads are over" << std::endl;

    sumNumCoprimes = boost::algorithm::reduce(numCoprimesBuffer, cpp_int(0));
    sumNumIterations = boost::algorithm::reduce(numIterationsBuffer, cpp_int(0));

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

void calculateValues(CalculationParams params)
{
    const uint32_t seed = std::random_device()();
    boost::random::mt11213b gen(seed);
    boost::array<uint32_t, 4096> buffer;

    constexpr auto middle = buffer.static_size / 2;
    BOOST_STATIC_ASSERT_MSG(buffer.static_size % 2 == 0, "buffer size must be even");

    const auto &[numOfCoprimes, iterations, limit] = params;

    while (*iterations < limit)
    {
        gen.generate(buffer.begin(), buffer.end());

        *numOfCoprimes += boost::algorithm::transform_reduce(
            buffer.begin(),
            buffer.begin() + middle,
            buffer.begin() + middle,
            0L,
            std::plus(),
            [](const auto &a, const auto &b)
            { return boost::integer::gcd(a, b) == 1; });

        *iterations += middle;
    }
}
