#include <hip/hip_runtime.h>
#include "hip_helper/memory.hpp"
#include "hip_helper/register.hpp"

#include <array>
#include <iostream>

template<typename T>
struct device_entry {
    __global__
    static void entry(auto* result, auto... params) {
        T t{};
        *result = t.invoke(params...);
    }
    template<typename... Params>
    static inline auto call(Params... params) {
        using Result = std::invoke_result_t<decltype(T::invoke), Params...>;
        auto device_res = hip_helper::device::array<Result, 1>{};
        entry<<<dim3(1,1,1),dim3(1,1,1),0>>>(device_res.data(), params...);
        return device_res[0];
    }

    auto operator()(auto... params) {
        return call(params...);
    }
};

int main() {
    auto [indices, values] = hip_helper::get_reg_values<1,2,3,4,5,6,7,8>();
    for (int i = 0; i < indices.size(); i++) {
        std::cout << std::dec << "Reg " << indices[i] << ": " << std::hex << values[i] << std::endl;
    }
    auto f = device_entry<hip_helper::get_exec<uint64_t>>{};
    auto exec = f();
    std::cout << "exec: " << exec << std::endl;
    return 0;
}
