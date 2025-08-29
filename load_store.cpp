#include <hip/hip_runtime.h>
#include <iostream>
#include "hip_helper/memory.hpp"

__global__
void load_store(int n, float* in, float* out) {
    int tid = threadIdx.x + blockDim.x * blockIdx.x;
    out[tid] = in[tid];
}

int main() {
    auto in = std::array<float, 32>{};
    auto out = in;
    for (int i = 0; i < in.size(); i++) {
        in[i] = sinf(i);
    }

    auto device_in = hip_helper::device::array<float, in.size()>{};
    auto device_out = hip_helper::device::array<float, out.size()>{};

    device_in = in;

    load_store<<<dim3(in.size(),1,1), dim3(1,1,1), 0>>>(device_in.size(), device_in.data(), device_out.data());

    out = device_out;

    for (int i = 0; i < out.size(); i++) {
        std::cout << out[i] << ',';
    }
    std::cout << std::endl;
    return 0;
}
