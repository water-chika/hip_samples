all: load_store dump_register

load_store: load_store.cpp Makefile
	amdclang++ --offload-arch=gfx1201 -x hip load_store.cpp -I /opt/rocm/include -std=c++20 -o load_store --save-temps -g
dump_register: dump_register.cpp Makefile
	amdclang++ --offload-arch=gfx1201 -x hip dump_register.cpp -I /opt/rocm/include -std=c++20 -o dump_register --save-temps -g

