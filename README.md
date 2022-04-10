# BigMPICompat

This is a small header-only library to support large MPI operations
that are part of MPI 4.0 with an MPI implementation that supports
standard 3.0 only.

The list of supported routines is incomplete. The following functions are added for MPI implementations that are 3.x:
- MPI_Type_contiguous_c
- MPI_Send_c
- MPI_Recv_c

We also implement the following. As MPICH 4.0.x has these functions, but fails in any large IO operation, we put the correct implementation in a namespace:
- BigMPICompat::MPI_File_write_at_c
- BigMPICompat::MPI_File_write_at_all_c
- BigMPICompat::MPI_File_write_ordered_c
