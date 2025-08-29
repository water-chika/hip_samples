#include "build/build.hpp"
#include <iostream>

int main() {
    auto build_hpp =
        std::make_shared<build::file_object>(
                "build/build.hpp"
        );
    auto build_cpp = std::make_shared<build::file_object>("build.cpp");
    
    auto hip_samples_build = std::make_shared<build::file_object>(
            "hip_samples_build",
            std::vector<std::weak_ptr<build::file_object>>{build_hpp, build_cpp},
            [](){
                std::cout << "building" << std::endl;
                auto command = "c++ build.cpp -o hip_samples_build -std=c++20";
                system(command);
                std::cout << command << std::endl;
            }
            );

    auto load_store_cpp = std::make_shared<build::file_object>("load_store.cpp");
    auto load_store = std::make_shared<build::file_object>(
            "load_store",
            std::vector<std::weak_ptr<build::file_object>>{load_store_cpp},
            []() {
                std::cout << "building" << std::endl;
                auto command = "amdclang++ -x hip --offload-arch=gfx1201 load_store.cpp -o load_store -std=c++20";
                system(command);
                std::cout << command << std::endl;
            }
            );

    auto dump_register_cpp = std::make_shared<build::file_object>("dump_register.cpp");
    auto dump_register = std::make_shared<build::file_object>(
            "dump_register",
            std::vector<std::weak_ptr<build::file_object>>{dump_register_cpp},
            []() {
                std::cout << "building" << std::endl;
                auto command = "amdclang++ -x hip --offload-arch=gfx1201 dump_register.cpp -o dump_register -std=c++20";
                system(command);
                std::cout << command << std::endl;
            }
            );

    auto graph = build::dependency_graph{};
    graph.add(hip_samples_build);
    graph.add(build_cpp);
    graph.add(build_hpp);
    graph.add(load_store_cpp);
    graph.add(load_store);
    graph.add(dump_register_cpp);
    graph.add(dump_register);

    graph.update();
    return 0;
}
