#include <hip/hip_runtime.h>
#include "hip_helper/memory.hpp"
#include "hip_helper/register.hpp"
#include "hip_helper/hip_helper.hpp"

#include <array>
#include <iostream>

int main() {
    auto [indices, values] = hip_helper::get_reg_values<1,2,3,4,5,6,7,8>();
    for (int i = 0; i < indices.size(); i++) {
        std::cout << std::dec << "Reg " << indices[i] << ": " << std::hex << values[i] << std::endl;
    }
    uint64_t exec = hip_helper::hybrid_call<hip_helper::get_exec<uint64_t>>{};
    std::cout << "exec: " << exec << std::endl;
    return 0;
}
