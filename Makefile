all: hip_samples_build
	./hip_samples_build

hip_samples_build: build.cpp libbuild/build.hpp
	c++ build.cpp -o hip_samples_build -std=c++23 -ltbb
