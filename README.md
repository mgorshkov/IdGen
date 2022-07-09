# IDs sequence

Create id sequence, built according to the following rules:

1. 1st id is "A1", 2nd is "A2", 3rd is "A3" etc. "B1" follows "A9".
   After "Z9" goes "A1-A1", then goes "A1-A2" and so on. "A2-A1" follows "A1-Z9".
2. Max id length is 10 groups 2 symbols each.
3. There should be no "D", "F", "G", "J", "M", "Q", "V" symbols and "0".
   A class should be created, which should implement the above rules.
   The class must allow:
   • Setting current id value
   • Incrementing current id and returning the new one
   Requirements:
1. thread-safety.
2. be compilable.
3. be cross-platform (be compilable by msvc/gcc/clang).
4. only standard C++ library must be used (C++ up to C++17 inclusive).

# Build
```
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
# Run
./IdGen