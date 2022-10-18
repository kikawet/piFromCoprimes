#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/integer/common_factor.hpp>
#include <boost/thread/thread.hpp>
#include <boost/smart_ptr/atomic_shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <fstream>
#include <random>
#include <vector>

using namespace boost::multiprecision;
using namespace std::chrono_literals;

using TypeNumTotalPair = std::pair<boost::shared_ptr<cpp_int>, boost::shared_ptr<cpp_int>>;

std::vector<TypeNumTotalPair> calculations;

void calculateValues(TypeNumTotalPair &pair, const cpp_int &limit);

std::ostream &outputPi(std::ostream &output, const cpp_int &numOfCoprimes, const cpp_int &totalNumbers);

int main() {
    const int numOfThreads = 5;
    const cpp_int &limit = (cpp_int(1) << 128);
    std::vector<boost::thread> threads;
    cpp_int numOfCoprimes, totalNumbers;

    for (int i = 0; i < numOfThreads; ++i) {
        calculations.emplace_back(boost::make_shared<cpp_int>(0), boost::make_shared<cpp_int>(0));
        threads.emplace_back(calculateValues, calculations[i], limit);
    }

    std::cout << "Pi = " << std::setprecision(20) << std::endl;

    while (!threads.empty()) {
        const auto firstRemove = std::remove_if(threads.begin(), threads.end(), [](auto &thread) {
            return thread.try_join_for(boost::chrono::seconds(1));
        });

        threads.erase(firstRemove, threads.end());

        boost::this_thread::sleep_for(boost::chrono::seconds(2));
        numOfCoprimes = 0, totalNumbers = 0;

        for (const auto &cal: calculations) {
            numOfCoprimes += *cal.first;
            totalNumbers += *cal.second;
        }

        outputPi(std::cout, numOfCoprimes, totalNumbers) << '\n';
    }

    std::cout << "All threads are over" << std::endl;

    numOfCoprimes = 0, totalNumbers = 0;
    for (const auto &cal: calculations) {
        numOfCoprimes += *cal.first;
        totalNumbers += *cal.second;
    }

    std::ofstream file("PI.txt");

    outputPi(
            file << "PI = " << std::setprecision(100),
            numOfCoprimes,
            totalNumbers
    ) << std::endl;

    file.close();
    std::cout << "Finished storing result into PI.txt" << std::endl;
}

std::ostream &outputPi(std::ostream &output, const cpp_int &numOfCoprimes, const cpp_int &totalNumbers) {
    return output << sqrt(
            6 / (
                    (cpp_dec_float_100) numOfCoprimes /
                    (cpp_dec_float_100) totalNumbers
            )
    );
}

void calculateValues(TypeNumTotalPair &pair, const cpp_int &limit) {
    std::random_device gen;
    boost::shared_ptr<cpp_int> numOfCoprimes = pair.first,
            iteration = pair.second;

    while (++*iteration < limit) {
        if (boost::integer::gcd(gen(), gen()) == 1)
            ++*numOfCoprimes;
    }
}
