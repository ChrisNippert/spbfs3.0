bin/spbfs: src/*.cpp src/*.hpp
	mkdir -p bin
	clang++ -std=c++26 -Wall -Wextra -O3 src/main.cpp -o $@