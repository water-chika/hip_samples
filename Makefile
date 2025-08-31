all: hip_samples_build
	./hip_samples_build

hip_samples_build: build.cpp build/build.hpp
	c++ build.cpp -o hip_samples_build -std=c++23 -ltbb

load_store: load_store.cpp Makefile
	amdclang++ --offload-arch=gfx1201 -x hip load_store.cpp -I /opt/rocm/include -std=c++20 -o load_store --save-temps -g
dump_register: dump_register.cpp Makefile
	amdclang++ --offload-arch=gfx1201 -x hip dump_register.cpp -I /opt/rocm/include -std=c++20 -o dump_register --save-temps -g

