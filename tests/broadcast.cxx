#include <big_mpi_compat.h>

#include "common.h"


void
test()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int      myid;
  MPI_Comm_rank(comm, &myid);

  const std::uint64_t count = (1ULL << 32) + 5;

  if (myid == 0)
    {
      std::vector<short> buffer(count, 0);
      buffer[1]         = 1;
      buffer[count - 1] = 99;
      int ierr          = BigMPICompat::Bcast_c(buffer.data(),
                                       count,
                                       MPI_SHORT,
                                       0, /* root */
                                       comm);
      CheckMPIFatal(ierr);
    }
  else
    {
      std::vector<short> buffer(count, 42);
      int                ierr = BigMPICompat::Bcast_c(buffer.data(),
                                       count,
                                       MPI_SHORT,
                                       0, /* root */
                                       comm);
      CheckMPIFatal(ierr);

      if (buffer[0] != 0 || buffer[1] != 1 || buffer[count - 1] != 99)
        {
          std::cerr << "MPI BCAST WAS INVALID:" << buffer[0] << ' ' << buffer[1]
                    << ' ' << buffer[count - 1] << std::endl;
          MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

  if (myid == 0)
    std::cout << "TEST Bcast: OK" << std::endl;
}

int
main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int myid, ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &ranks);

  assert(ranks >= 2);

  test();

  MPI_Finalize();
  return 0;
}
