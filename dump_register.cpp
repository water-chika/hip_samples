#include <hip/hip_runtime.h>
#include <iostream>
#include "hip_helper/memory.hpp"

__device__
uint32_t get_reg_value(int i) {
    uint32_t v;
    asm("s_getreg_b32 %0, hwreg(HW_REG_STATE_PRIV, 0, 32)" : "=s"(v));
    return v;
}

__global__
void device_get_reg_value(uint32_t* out) {
    out[0] = get_reg_value(0);
}

int main() {
    auto v = hip_helper::device::array<uint32_t, 1>{};
    device_get_reg_value<<<dim3(1,1,1),dim3(1,1,1),0>>>(v.data());
    auto reg_values = std::array<uint32_t, 1>{};
    reg_values = v;
    std::cout << reg_values[0] << std::endl;
    return 0;
}
