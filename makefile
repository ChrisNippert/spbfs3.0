bin/spbfs: src/*.cpp src/*.hpp
	mkdir -p bin
	clang++ -std=c++26 -Wall -Wextra -O3 src/main.cpp -o $@

bin/spbfs_debug:  src/*.cpp src/*.hpp
	mkdir -p bin
	clang++ -std=c++26 -Wall -Wextra -g -fsanitize=address,undefined -O1 src/main.cpp -o $@


clean:
	rm -rf ./bin/spbfs