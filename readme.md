## Requirements

- A C++17 compiler
- CMake ≥ 3.16

## Build

```bash
cmake -B build          #configure (run once)
cmake --build build -j  #compile
```
The first command sets up the build in a separate `build/` folder.
The second command compiles the code.

## Run

```bash
./build/kst m n s t
```

## Output
A Graph6 representation of the final graph and the number of edges in this graph