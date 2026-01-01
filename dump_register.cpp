#include <hip/hip_runtime.h>
#include "hip_helper/memory.hpp"
#include "hip_helper/register.hpp"
#include "hip_helper.hpp"

#include <array>
#include <iostream>
#include <execution>
#include <utility>

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

enum class function : uint32_t {
    f,
    print_str,
};

void f() {
    printf("call f");
}
void print_str(void* ptr) {
    printf("%s", static_cast<const char*>(ptr));
}

struct device_call_host {
    uint32_t call_status; // 0 is idle, 1 is call, 2 is complete
    function function_id;
    uint32_t params[512];
};

void call_function(device_call_host* call_info) {
    switch (call_info->function_id){
    case function::f:
        f();
        break;
    case function::print_str:
        print_str(call_info->params);
        break;
    }
}

__managed__ device_call_host* device_call_host_ptr=0;

__device__
void f() {
    device_call_host_ptr->function_id = function::f;
    device_call_host_ptr->call_status = 1;
    while (device_call_host_ptr->call_status != 2) {
        asm("s_sleep 127");
    }
    device_call_host_ptr->call_status = 0;
}

template<size_t N>
__device__
void print_str(const char (&str)[N]) {
    device_call_host_ptr->function_id = function::print_str;
    memcpy(device_call_host_ptr->params,str, N);
    device_call_host_ptr->call_status = 1;
    while (device_call_host_ptr->call_status != 2) {
        asm("s_sleep 127");
    }
    device_call_host_ptr->call_status = 0;
}

struct test {
    __device__
    static inline uint32_t invoke() {
        print_str("hello");
        return 0;
    }
    __device__
    test() {
        invoke();
    }
    __device__
    operator uint32_t() {
        return 0;
    }
};

template class hip_helper::hybrid_call<test>;

int main() {
    auto [indices, values] = hip_helper::get_reg_values<1,2,3,4,5,6,7,8>();
    for (int i = 0; i < indices.size(); i++) {
        std::cout << std::dec << "Reg " << indices[i] << ": " << std::hex << values[i] << std::endl;
    }
    uint64_t exec = hip_helper::hybrid_call<hip_helper::get_exec<uint64_t>>{};
    std::cout << "exec: " << exec << std::endl;

    uint64_t real_time = hip_helper::hybrid_call<get_realtime>{};
    std::cout << "real time: " << real_time << std::endl;

    auto seq = std::array{0, 1};

    auto res = hipMallocManaged(&device_call_host_ptr, sizeof(*device_call_host_ptr), hipMemAttachGlobal);
    assert(res == 0);

    std::for_each(
            std::execution::par,
            seq.begin(), seq.end(),
            [](auto i) {
                if (i == 0) {
                    hip_helper::hybrid_call<test>{};
                }
                else {
                    while (device_call_host_ptr->call_status == 0) {}
                    assert(device_call_host_ptr->call_status == 1);
                    std::cout << "device call host function: " << uint32_t(device_call_host_ptr->function_id) << std::endl;
                    call_function(device_call_host_ptr);
                    device_call_host_ptr->call_status = 2;
                }
            }
            );

    return 0;
}
