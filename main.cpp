#include <iostream>
#include <random>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <fstream>
#include <vector>
#include <utility>
#include <thread>
#include <future>
#include <chrono>
#include <numeric>

using namespace boost::multiprecision;
using namespace std::chrono_literals;

std::vector<std::pair<cpp_int &, cpp_int &>> calculations;

void calculateValues(int index, const cpp_int &limit);

cpp_dec_float_100 getPi(const cpp_int &numOfCoprimes, const cpp_int &totalNumbers);

int main() {
    const int numOfThreads = 5;
    cpp_int limit = (cpp_int(1) << 128);
    std::vector<std::future<void>> threads;

    for (int i = 0; i < numOfThreads; ++i) {
        threads.push_back(std::async(std::launch::async, [&]() {
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
            numOfCoprimes += cal.first;
            totalNumbers += cal.second;
        }

        std::cout << getPi(numOfCoprimes, totalNumbers) << std::endl;
    }

    std::cout << "All threads are over" << std::endl;

    numOfCoprimes = 0, totalNumbers = 0;
    for (const auto &cal: calculations) {
        numOfCoprimes += cal.first;
        totalNumbers += cal.second;
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
    std::random_device rd;
    cpp_int numOfCoprimes = 0, iteration = 0;
    calculations[index] = std::make_pair(numOfCoprimes, iteration);

    while (++iteration < limit) {
        if (std::gcd(rd(), rd()) == 1)
            ++numOfCoprimes;
    }
}
