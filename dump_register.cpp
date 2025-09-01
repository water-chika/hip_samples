#include <hip/hip_runtime.h>
#include "hip_helper/memory.hpp"

#include <array>
#include <iostream>

template<uint32_t HW_REG_INDEX>
__device__
uint32_t get_reg_value() {
    uint32_t v;
    asm("s_getreg_b32 %0, hwreg(%1, 0, 32)" : "=s"(v) : "i"(HW_REG_INDEX));
    return v;
}

template<uint32_t V, uint32_t... Vs>
struct parameters_call {
    __device__
    static void call(uint32_t* out) {
        out[V] = get_reg_value<V>();
        parameters_call<Vs...>::call(out);
    }
};
template<uint32_t V>
struct parameters_call<V> {
    __device__
    static void call(uint32_t* out) {
        out[V] = get_reg_value<V>();
    }
};

template<uint32_t I, uint32_t... Indices>
__global__
void device_get_reg_value(uint32_t* out) {
    parameters_call<I, Indices...>::call(out);
}

template<uint32_t N, uint32_t... Ns>
auto get_reg_values() {
    constexpr auto reg_indices = std::array{N, Ns...};
    auto reg_values = std::array<uint32_t, reg_indices.size()>{};
    {
        auto v = hip_helper::device::array<uint32_t, reg_values.size()>{};
        device_get_reg_value<N, Ns...><<<dim3(1,1,1),dim3(1,1,1),0>>>(v.data());
        reg_values = v;
    }
    return std::pair{reg_indices, reg_values};
}

int main() {
    auto [indices, values] = get_reg_values<1,2,3,4,5,6,7,8>();
    for (int i = 0; i < indices.size(); i++) {
        std::cout << std::dec << "Reg " << indices[i] << ": " << std::hex << values[i] << std::endl;
    }
    return 0;
}
