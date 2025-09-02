#include "build/build.hpp"
#include <iostream>
#include <format>
#include <ranges>
#include <set>

int main() {
    auto builder = build::builder{};

    builder.add_executable("hip_samples_build", "build.cpp", "build/build.hpp");
    builder.add_hip_executable("load_store", "load_store.cpp");
    builder.add_hip_executable("dump_register", "dump_register.cpp");

    return builder.build() == build::update_res::success ? 0 : -1;
}
