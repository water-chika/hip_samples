#include "build/build.hpp"
#include <iostream>
#include <format>
#include <ranges>

auto c_plus_plus_compile(const std::vector<std::string>& src, const std::string& output_path) {
    std::string sources = "";
    std::ranges::for_each(
            src,
            [&sources](auto s) {
                sources += " " + s;
            }
            );
    auto command = std::format("c++ {} -o {} -std=c++23 -ltbb", sources, output_path);
    std::cout << command << std::endl;
    return system(command.c_str());
}
auto c_plus_plus_compile(const std::string src, const std::string& output_path) {
    return c_plus_plus_compile(std::vector<std::string>{src}, output_path);
}
auto amdclang_plus_plus_compile(const std::vector<std::string>& src, const std::string& output_path) {
    auto command = std::format("amdclang++ -x hip --offload-arch=gfx1201 {} -o {} -std=c++20", src, output_path);
    std::cout << command << std::endl;
    return system(command.c_str());
}

int main() {
    auto build_hpp = build::file("build/build.hpp");
    auto build_cpp = build::file("build.cpp");
    
    auto hip_samples_build = build::file("hip_samples_build", build::dependencies(build_hpp, build_cpp),
            [](const auto& target){
                auto weak_sources = target.get_dependencies();
                auto sources = std::vector<std::shared_ptr<build::file_object>>(weak_sources.size());
                std::ranges::transform(
                        weak_sources,
                        sources.begin(),
                        [](auto e) {
                            return e.lock();
                        }
                        );
                auto sources_string = std::vector<std::string>(sources.size());
                std::ranges::transform(
                        sources,
                        sources_string.begin(),
                        [](auto& src) {
                            return src->get_file_path().string();
                        }
                        );
                auto res = c_plus_plus_compile(sources_string, target.get_file_path());
                return res == 0 ? build::update_res::success : build::update_res::failed;
            }
            );

    auto load_store_cpp = build::file("load_store.cpp");
    auto load_store = build::file(
            "load_store",
            build::dependencies(load_store_cpp),
            [](auto) {
                std::cout << "building" << std::endl;
                auto command = "amdclang++ -x hip --offload-arch=gfx1201 load_store.cpp -o load_store -std=c++20";
                auto res = system(command);
                std::cout << command << std::endl;
                return res == 0 ? build::update_res::success : build::update_res::failed;
            }
            );

    auto dump_register_cpp = build::file("dump_register.cpp");
    auto hip_helper_memory_hpp = build::file("hip_helper/memory.hpp");
    auto hip_helper_register_hpp = build::file("hip_helper/register.hpp");
    auto dump_register = build::file(
            "dump_register",
            build::dependencies(dump_register_cpp, hip_helper_memory_hpp, hip_helper_register_hpp),
            [](auto) {
                std::cout << "building" << std::endl;
                auto command = "amdclang++ -g -x hip --offload-arch=gfx1201 dump_register.cpp -o dump_register -std=c++20";
                auto res = system(command);
                std::cout << command << std::endl;
                return res == 0 ? build::update_res::success : build::update_res::failed;
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
