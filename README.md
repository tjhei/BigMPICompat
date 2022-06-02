# BigMPICompat

This is a small header-only library to support large MPI operations
that are part of MPI 4.0 with an MPI implementation that supports
standard 3.0 only.

## Features

The list of supported routines is incomplete. The following functions
are added for MPI implementations that are 3.x:
- BigMPICompat::Type_contiguous_c
- BigMPICompat::Send_c
- BigMPICompat::Recv_c

We also implement the following. As MPICH 4.0.x has these functions, but fails in any large IO operation, we supply an alternative implementatin for it as well:
- BigMPICompat::File_write_at_c
- BigMPICompat::File_write_at_all_c
- BigMPICompat::File_write_ordered_c

## About

- Usage: just include the single header file in your code
- Requirements: OpenMPI 3.0.x or MPICH 3.1.x or newer

This projects was made to allow for large MPI communication in the [deal.II](https://dealii.org) library. The project is heavily inspired by the [BigMPI library](https://github.com/jeffhammond/BigMPI).


## Test results

| version        | MPI support | native large transfer | native large IO           | tests                                   |
|----------------|-------------|-----------------------|---------------------------|-----------------------------------------|	
| MPICH 3.0.4    |         3.0 |                       |                           | FAIL: MPI_Type_contiguous_c [1]         |
| MPICH 3.1.4    |         3.0 |                       |                           | OK                                      |
| MPICH 3.2.1    |         3.1 |                       |                           | OK                                      |
| MPICH 3.3.2    |         3.1 |                       |                           | OK                                      |
| MPICH 3.4.3    |         3.1 |                       |                           | OK                                      |
| MPICH 4.0.1    |         4.0 | OK                    | FAIL: Assert inside MPICH | OK                                      |
| OpenMPI 1.8.8  |         3.0 |                       |                           | FAIL: MPI_Type_contiguous_c [2]         |
| OpenMPI 1.10.7 |         3.0 |                       |                           | FAIL: MPI_Type_contiguous_c [2]         |
| OpenMPI 2.0.4  |         3.1 |                       |                           | FAIL: MPI_Type_contiguous_c [2]         |
| OpenMPI 2.1.6  |         3.1 |                       |                           | FAIL: MPI_Type_contiguous_c [2]         |
| OpenMPI 3.0.5  |         3.1 |                       |                           | OK                                      |
| OpenMPI 3.1.6  |         3.1 |                       |                           | OK                                      |
| OpenMPI 4.0.7  |         3.1 |                       |                           | OK                                      |
| OpenMPI 4.1.3  |         3.1 |                       |                           | OK                                      |
|                |             |                       |                           |                                         |

[1]: Maximum usable value is count = (1<<31) for MPICH.

[2]: Maximum usable value is count = (1<<32) for OpenMPI.