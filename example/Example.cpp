/***************************************************************
 *
 * This is a simple example showing how to use the library.
 *
 *
 * Copyright 2011 Nicolas Bigaouette <nbigaouette@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * https://github.com/nbigaouette/oclutils
 ***************************************************************/

#include <OclUtils.hpp>

// **************************************************************
int main(int argc, char *argv[])
{
    // Declare an object containing a list of OpenCL platforms.
    OpenCL_platforms_list platforms_list;

    // Try to find the best OpenCL device on the wanted platform,
    // then lock it for exclusive usage, and print informations.

    // Initialize the list of platforms. The argument is the preferred platform.
    // Values (std::string) can be:
    //      "amd"       AMD Accelerated Parallel Processing (APP), aka "AMD Stream SDK"
    //                  Support ATI GPU and x86/x86_64 CPU
    //                  http://developer.amd.com/gpu/ATIStreamSDK/Pages/default.aspx
    //      "intel"     Intel OpenCL SDK
    //                  http://www.intel.com/go/opencl/
    //      "nvidia"    NVIDIA CUDA OpenCL
    //                  http://developer.nvidia.com/opencl
    //      "apple"     Apple OpenCL (only on MacOSX)
    //      "-1"        Default value. This will take the first
    //                  platform available, in alphabetical order.
    // By default, the library will use lock files to prevent multiple
    // programs from using a single devices. If you want to prevent
    // that, append "false" to Initialize()'s arguments.
    platforms_list.Initialize("-1");

    // By passing "-1", to Initialize(), the first platform in the list
    // is chosen. Get that platform's string for later reference.
    const std::string platform = platforms_list.Get_Running_Platform();

    // Lock the best device available on the platform. Devices are
    // ordered by number of max compute units (CL_DEVICE_MAX_COMPUTE_UNITS).
    // GPUs normally have at least a couple dozen compute units. On
    // CPUs, CL_DEVICE_MAX_COMPUTE_UNITS is the number of cores.
    // The locking is done by creating the file /tmp/gpu_usage.txt
    // where the platform and device is saved.
    platforms_list[platform].Lock_Best_Device();

    // Print All information possible on the platforms and their devices.
    platforms_list.Print();

    // Create a command queue on "platform"'s preferred device.
    cl_int err;
    cl::CommandQueue command_queue(  
        platforms_list[platform].Preferred_OpenCL_Device_Context(),  // OpenCL context
        platforms_list[platform].Preferred_OpenCL_Device(),          // OpenCL device id
        0, &err);

    // Pause
    std::string answer;
    std::cout << "Press enter to exit example. The OpenCL device will be released.\n";
    getline(std::cin, answer);

    return EXIT_SUCCESS;
}

