cmake_minimum_required(VERSION 3.20)

project(hip_samples)

add_executable(build_samples
	build.cpp)

add_subdirectory(libbuild)

target_link_libraries(build_samples PUBLIC build)

add_executable(dump_register
	dump_register.cpp)

add_subdirectory(hip_helper)

target_link_libraries(dump_register PUBLIC hip_helper)
target_compile_features(dump_register PUBLIC cxx_std_20)
