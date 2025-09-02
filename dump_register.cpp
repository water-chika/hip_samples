#include <hip/hip_runtime.h>
#include "hip_helper/memory.hpp"
#include "hip_helper/register.hpp"
#include "hip_helper/hip_helper.hpp"

#include <array>
#include <iostream>

struct get_realtime {
    __device__
    static inline uint64_t invoke() {
        uint64_t realtime;
        asm(
            "s_sendmsg_rtn_b64 %0 0x83\n"
            "s_wait_kmcnt 0"
                : "=s"(realtime));
        return realtime;
    }
    __device__
    get_realtime() {
        m_res = invoke();
    }

    __device__
    operator uint64_t() {
        return m_res;
    }

    uint64_t m_res;
};

int main() {
    auto [indices, values] = hip_helper::get_reg_values<1,2,3,4,5,6,7,8>();
    for (int i = 0; i < indices.size(); i++) {
        std::cout << std::dec << "Reg " << indices[i] << ": " << std::hex << values[i] << std::endl;
    }
    uint64_t exec = hip_helper::hybrid_call<hip_helper::get_exec<uint64_t>>{};
    std::cout << "exec: " << exec << std::endl;

    uint64_t real_time = hip_helper::hybrid_call<get_realtime>{};
    std::cout << "real time: " << real_time << std::endl;
    return 0;
}
