cd cmake-build-debug || exit
conan install ..
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
