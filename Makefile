load_store:
	amdclang++ -x hip load_store.cpp -I /opt/rocm/include -std=c++20 -o load_store
