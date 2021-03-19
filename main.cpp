#include <iostream>
#include <random>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <cmath>
#include <fstream>
#include <vector>
#include <utility>
#include <thread>
#include <future>
#include <chrono>
#include <numeric>

using namespace boost::multiprecision;
using namespace std::chrono_literals;

std::vector<std::pair<cpp_int, cpp_int>> calculations;

void calculateValues(int index, cpp_int limit, cpp_int logPeriod);

int main() {
  const int numOfThreads = 5;
  cpp_int limit = (cpp_int(1) << 128)
      , updatePeriod = (cpp_int(1) << 16);
  std::vector<std::future<void>> threads;
  
  for(int i = 0; i < numOfThreads; ++i){    
    threads.push_back(std::move(std::async(std::launch::async,[&](){
      calculateValues(i, limit, updatePeriod);
    })));
        
    calculations.push_back(std::make_pair(0,0));
  }

  std::cout << "Pi = " << std::setprecision(20) << std::endl;
  cpp_int numOfCoprimes, totalNumbers;
  
  while(threads.size()){
    for(int i = threads.size()-1; i >= 0; --i){
      if(threads[i].wait_for(1s) == std::future_status::ready){
        threads.erase(threads.begin() + i);
      }
    }

    std::this_thread::sleep_for(2s);
    numOfCoprimes = 0, totalNumbers = 0;
    for(const auto& cal: calculations){
      numOfCoprimes += cal.first;
      totalNumbers += cal.second;
    }

    std::cout << sqrt(
        6/(
          (cpp_dec_float_100)numOfCoprimes/
          (cpp_dec_float_100)totalNumbers
        )
      ) << std::endl;
  }

  std::cout << "All threads are over" << std::endl;

  numOfCoprimes = 0, totalNumbers = 0;
  for(const auto& cal: calculations){
    numOfCoprimes += cal.first;
    totalNumbers += cal.second;
  }

  std::ofstream file("PI.txt");


  file << "PI = " << std::setprecision(100) << sqrt(
        6/(
          (cpp_dec_float_100)numOfCoprimes/
          (cpp_dec_float_100)totalNumbers
        )
      ) << std::endl;

file.close();
std::cout << "Finished storing result into PI.txt" << std::endl;
}

void calculateValues(int index, cpp_int limit, cpp_int logPeriod){
  std::random_device rd;
  cpp_int numOfCoprimes = 0, iteration = 0;
  
  while(++iteration < limit){
    if(std::gcd(rd(),rd()) == 1)
      ++numOfCoprimes;    

    if(iteration%logPeriod == 0)
      calculations[index] = std::make_pair(numOfCoprimes,iteration);
  }
}
