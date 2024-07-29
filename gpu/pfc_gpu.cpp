#define CL_TARGET_OPENCL_VERSION 300
#define BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION
#define BOOST_COMPUTE_USE_OFFLINE_CACHE
#include <vector>
#include <cstdlib>
#include <iostream>
#include <random>

#include <boost/compute.hpp>
#include <boost/random.hpp>

#include <SFMT.h>

namespace compute = boost::compute;
using boost::compute::lambda::_1;
using boost::compute::lambda::_2;

int main()
{
    sfmt_t sfmt;
    // get the default device
    compute::device gpu = compute::system::default_device();

    std::cout
        << "-------------------------------\n"
        << "Using device:" << gpu.name() << std::endl;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // create context for default device
    compute::context context(gpu);

    // create command queue with profiling enabled
    compute::command_queue queue(context, gpu);

    constexpr uint32_t randomsBufferSize = 200'000'000;
    compute::vector<uint32_t> device_vector(randomsBufferSize * 2, context);
    std::vector<uint32_t> host_vector(device_vector.size());
    compute::vector<uint32_t> device_res(randomsBufferSize, context);

    const uint32_t seed = std::random_device()();
    boost::random::mt11213b gen(seed);

    gen.generate(host_vector.begin(), host_vector.end());

    std::chrono::steady_clock::time_point begin_copy = std::chrono::steady_clock::now();

    compute::copy(
        host_vector.begin(), host_vector.end(), device_vector.begin(), queue);

    std::chrono::steady_clock::time_point end_copy = std::chrono::steady_clock::now();

    BOOST_COMPUTE_FUNCTION(uint, coprimes_check, (uint a, uint b),
                           {
                               uint atz = ctz(a);
                               uint btz = ctz(b);
                               uint shift = min(atz, btz);
                               b >>= btz;

                               while (a != 0)
                               {
                                   a >>= atz;
                                   int diff = b - a;
                                   atz = ctz(diff);
                                   b = min(a, b);
                                   a = abs(diff);
                               }

                               return (b << shift) == 1;
                           });

    std::cout << "Kernel compiled starting to crunch numbers" << std::endl;

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    // engine.generate(device_vector.begin(), device_vector.end(), queue);

#if 1

    compute::transform_reduce(device_vector.begin(),
                              device_vector.begin() + randomsBufferSize,
                              device_vector.begin() + randomsBufferSize,
                              device_res.begin(),
                              coprimes_check,
                              compute::plus<uint32_t>(),
                              queue);

#else
    compute::transform(device_vector.begin(),
                       device_vector.begin() + randomsBufferSize,
                       device_vector.begin() + randomsBufferSize,
                       device_res.begin(),
                       coprimes_check,
                       queue);

    compute::partial_sum(
        device_res.begin(),
        device_res.end(),
        device_res.begin(),
        queue);

#endif
    std::cout << "Waiting for GPU to finish" << std::endl;

    queue.finish();

    // copy data from the host to the device

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    // print elapsed time in milliseconds

    std::cout << "Generated\t" << randomsBufferSize * 2 << " numbers\n"
              << "Added\t\t" << randomsBufferSize << " pairs\n"
              << "Time program\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n"
              << "Time computing\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "[ms]\n"
              << "Time coping\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end_copy - begin_copy).count() << "[ms]\n"
              << "Total coprimes\t" << *device_res.begin() << std::endl;

    return 0;
}