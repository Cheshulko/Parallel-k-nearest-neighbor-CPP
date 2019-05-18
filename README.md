# Parallel _k_-Nearest Neighbor Algorithm

## Overview
It is c++ parallel implementation of the k-nearest neighbor algorithm using async c++ threads. The data are evenly split between _n_ threads that each thread computes the _k_-nearest neighbor for its part of data.

## Input
A input file _test_big.txt_ contains coordinates of 9271 _3D_-points in the world space.

## Output
For each input point, find its _k_-nearest neighbors indexes. 

## Build and run
To build use Makefile (C++11 needed)
```
make
make clean
./main <input_points.txt> <result.txt>
```
For instance,
```
./main ./input/test_big.txt ./out/out_big.txt
```

## Benchmarks
Intel(R) Xeon(R) CPU 2.00GHz

For 9271 _3D_-points and _k_=5

| Threads  | Time, milisec |
| -------  |---|
| 1  | 45979 |
| 2  | 24430 |
| 3  | 18370 |
| 4  | 14112 |
| 5  | 13207 |

## License
MIT
