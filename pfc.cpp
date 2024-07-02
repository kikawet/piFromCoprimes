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

using SharedInt = boost::shared_ptr<cpp_int>;

std::vector<SharedInt> numOfCoprimes;
std::vector<SharedInt> numIterations;

void calculateValues(const SharedInt numOfCoprimes, const SharedInt iterations, const cpp_int &limit);
std::ostream &outputPi(std::ostream &output, const cpp_int &numOfCoprimes, const cpp_int &totalNumbers);
cpp_int add(const std::vector<SharedInt> &nums);

int main()
{
    const int numOfThreads = 5;
    const cpp_int &limit = (cpp_int(1) << 128);
    std::vector<boost::thread> threads;

    for (int i = 0; i < numOfThreads; ++i)
    {
        numOfCoprimes.emplace_back(boost::make_shared<cpp_int>(0));
        numIterations.emplace_back(boost::make_shared<cpp_int>(0));
        threads.emplace_back(calculateValues, numOfCoprimes[i], numIterations[i], limit);
    }

    std::cout << "Pi = " << std::setprecision(20) << std::endl;

    cpp_int sumNumOfCoprimes, sumNumIterations;
    while (!threads.empty())
    {
        const auto firstRemove = std::remove_if(threads.begin(), threads.end(), [](auto &thread)
                                                { return thread.try_join_for(boost::chrono::seconds(1)); });

        threads.erase(firstRemove, threads.end());

        boost::this_thread::sleep_for(boost::chrono::seconds(2));

        sumNumOfCoprimes = add(numOfCoprimes);
        sumNumIterations = add(numIterations);

        outputPi(std::cout, sumNumOfCoprimes, sumNumIterations) << '\n';
    }

    std::cout << "All threads are over" << std::endl;

    sumNumOfCoprimes = add(numOfCoprimes);
    sumNumIterations = add(numIterations);

    std::ofstream file("PI.txt");

    outputPi(
        file << "PI = " << std::setprecision(100),
        sumNumOfCoprimes,
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

void calculateValues(const SharedInt numOfCoprimes, const SharedInt iterations, const cpp_int &limit)
{
    std::random_device rd;
    boost::random::mt11213b gen(rd());
    boost::array<uint32_t, 4096> buffer;

    constexpr auto middle = buffer.static_size / 2;
    BOOST_STATIC_ASSERT_MSG(buffer.static_size % 2 == 0, "buffer size must be even");

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

struct sum
{
    cpp_int operator()(const SharedInt &a, const SharedInt &b) const { return *a + *b; }
    cpp_int operator()(const cpp_int &a, const SharedInt &b) const { return a + *b; }
};

cpp_int add(const std::vector<SharedInt> &nums)
{
    return boost::algorithm::reduce(nums.begin(), nums.end(), cpp_int(0), sum());
}
