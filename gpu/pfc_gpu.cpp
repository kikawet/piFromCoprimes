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
    constexpr uint32_t randomsBufferSize = 200'000'000;

    sfmt_t sfmt;
    if (sfmt_get_min_array_size32(&sfmt) > randomsBufferSize)
    {
        std::cerr << "array size too small!" << std::endl;
        return EXIT_FAILURE;
    }

    const uint32_t seed = std::random_device()();
    sfmt_init_gen_rand(&sfmt, seed);

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

    compute::vector<uint32_t> device_vector(randomsBufferSize * 2, context);
    compute::vector<uint32_t> device_res(randomsBufferSize, context);

    std::chrono::steady_clock::time_point begin_generate = std::chrono::steady_clock::now();

    uint32_t *host_vector = static_cast<uint32_t *>(std::aligned_alloc(16, sizeof(uint32_t) * randomsBufferSize * 2));

    sfmt_fill_array32(&sfmt, host_vector, device_vector.size());

    std::chrono::steady_clock::time_point end_generate = std::chrono::steady_clock::now();

    std::chrono::steady_clock::time_point begin_copy = std::chrono::steady_clock::now();

    compute::copy(
        host_vector, host_vector + device_vector.size(), device_vector.begin(), queue);

    std::cout << "Randoms generated starting to compile kernel" << std::endl;

    std::chrono::steady_clock::time_point end_copy = std::chrono::steady_clock::now();

    // https://en.algorithmica.org/hpc/algorithms/gcd/#binary-gcd
    BOOST_COMPUTE_FUNCTION(uint, coprimes_check, (uint a, uint b),
                           {
                               if (a == 0)
                                   return b;
                               if (b == 0)
                                   return a;

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

    compute::transform_reduce(device_vector.begin(),
                              device_vector.begin() + randomsBufferSize,
                              device_vector.begin() + randomsBufferSize,
                              device_res.begin(),
                              coprimes_check,
                              compute::plus<uint32_t>(),
                              queue);

    std::cout << "Waiting for GPU to finish" << std::endl;

    queue.finish();
    std::free(host_vector);

    // copy data from the host to the device
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    const uint32_t totalCoprimes = *device_res.begin();

    // print elapsed time in milliseconds
    std::cout << "Generated\t" << device_vector.size() << " numbers\n"
              << "Added\t\t" << randomsBufferSize << " pairs\n"
              << "Time program\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n"
              << "Time generating\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end_generate - begin_generate).count() << "[ms]\n"
              << "Time coping\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end_copy - begin_copy).count() << "[ms]\n"
              << "Time computing\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "[ms]\n"
              << "Total coprimes\t" << totalCoprimes << '\n'
              << "PI\t\t" << std::sqrt(6. / ((double)totalCoprimes / (double)randomsBufferSize)) << std::endl;

    return 0;
}