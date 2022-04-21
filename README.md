# BigMPICompat

This is a small header-only library to support large MPI operations
that are part of MPI 4.0 with an MPI implementation that supports
standard 3.0 only.

## Features

The list of supported routines is incomplete. The following functions
are added for MPI implementations that are 3.x:
- MPI_Type_contiguous_c
- MPI_Send_c
- MPI_Recv_c

We also implement the following. As MPICH 4.0.x has these functions, but fails in any large IO operation, we put the correct implementation in a namespace:
- BigMPICompat::MPI_File_write_at_c
- BigMPICompat::MPI_File_write_at_all_c
- BigMPICompat::MPI_File_write_ordered_c

## About

- Usage: just include the single header file in your code
- Requirements: OpenMPI 3.0.x or MPICH 3.1.x or newer

This projects was made to allow for large MPI communication in the [https://dealii.org](deal.II) library. The project is heavily inspired by the [https://github.com/jeffhammond/BigMPI](BigMPI library).


## Test results

| version        | MPI support | native large transfer | native large IO           | tests                                   |
|----------------|-------------|-----------------------|---------------------------|-----------------------------------------|
| MPICH 3.0.4    |         3.0 |                       |                           | FAIL: MPI_Tye_contiguous_c wrong result |
| MPICH 3.1.4    |         3.0 |                       |                           | OK                                      |
| MPICH 3.2.1    |         3.1 |                       |                           | OK                                      |
| MPICH 3.3.2    |         3.1 |                       |                           | OK                                      |
| MPICH 3.4.3    |         3.1 |                       |                           | OK                                      |
| MPICH 4.0.1    |         4.0 | OK                    | FAIL: Assert inside MPICH | OK                                      |
| OpenMPI 1.8.8  |         3.0 |                       |                           | FAIL: MPI_Tye_contiguous_c wrong result |
| OpenMPI 1.10.7 |         3.0 |                       |                           | FAIL: MPI_Tye_contiguous_c wrong result |
| OpenMPI 2.0.4  |         3.1 |                       |                           | FAIL: MPI_Tye_contiguous_c wrong result |
| OpenMPI 2.1.6  |         3.1 |                       |                           | FAIL: MPI_Tye_contiguous_c wrong result |
| OpenMPI 3.0.5  |         3.1 |                       |                           | OK                                      |
| OpenMPI 3.1.6  |         3.1 |                       |                           | OK                                      |
| OpenMPI 4.0.7  |         3.1 |                       |                           | OK                                      |
| OpenMPI 4.1.3  |         3.1 |                       |                           | OK                                      |
|                |             |                       |                           |                                         |

