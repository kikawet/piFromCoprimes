#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/integer/common_factor.hpp>
#include <fstream>
#include <random>
#include <vector>
#include <thread>
#include <future>
#include <chrono>

using namespace boost::multiprecision;
using namespace std::chrono_literals;

std::vector<std::pair<std::shared_ptr<cpp_int>, std::shared_ptr<cpp_int>>> calculations;

void calculateValues(int index, const cpp_int &limit);

cpp_dec_float_100 getPi(const cpp_int &numOfCoprimes, const cpp_int &totalNumbers);

int main() {
    const int numOfThreads = 5;
    cpp_int limit = (cpp_int(1) << 128);
    std::vector<std::future<void>> threads;

    calculations.resize(numOfThreads,
                        std::make_pair(std::make_shared<cpp_int>(0), std::make_shared<cpp_int>(0)));

    for (int i = 0; i < numOfThreads; ++i) {
        threads.push_back(std::async(std::launch::async, [&limit, i]() {
            calculateValues(i, limit);
        }));
    }

    std::cout << "Pi = " << std::setprecision(20) << std::endl;
    cpp_int numOfCoprimes, totalNumbers;

    while (!threads.empty()) {
        const auto firstRemove = std::remove_if(threads.begin(), threads.end(), [](const auto &thread) {
            return thread.wait_for(1s) == std::future_status::ready;
        });

        threads.erase(firstRemove, threads.end());

        std::this_thread::sleep_for(2s);
        numOfCoprimes = 0, totalNumbers = 0;

        for (const auto &cal: calculations) {
            numOfCoprimes += *cal.first;
            totalNumbers += *cal.second;
        }

        std::cout << getPi(numOfCoprimes, totalNumbers) << std::endl;
    }

    std::cout << "All threads are over" << std::endl;

    numOfCoprimes = 0, totalNumbers = 0;
    for (const auto &cal: calculations) {
        numOfCoprimes += *cal.first;
        totalNumbers += *cal.second;
    }

    std::ofstream file("PI.txt");


    file << "PI = " << std::setprecision(100) << getPi(numOfCoprimes, totalNumbers) << std::endl;

    file.close();
    std::cout << "Finished storing result into PI.txt" << std::endl;
}

cpp_dec_float_100 getPi(const cpp_int &numOfCoprimes, const cpp_int &totalNumbers) {
    return sqrt(
            6 / (
                    (cpp_dec_float_100) numOfCoprimes /
                    (cpp_dec_float_100) totalNumbers
            )
    );
}

void calculateValues(int index, const cpp_int &limit) {
    std::random_device gen;
    std::shared_ptr<cpp_int> numOfCoprimes = calculations[index].first,
            iteration = calculations[index].second;

    while (++*iteration < limit) {
        if (boost::integer::gcd(gen(), gen()) == 1)
            ++*numOfCoprimes;
    }
}
