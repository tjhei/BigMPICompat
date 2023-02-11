#include <big_mpi_compat.h>

#include "common.h"

#if MPI_VERSION >= 4

void
test_send_recv()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int      myid;
  MPI_Comm_rank(comm, &myid);

  const std::uint64_t n_bytes = (1ULL << 32) + 5;

  if (myid == 0)
    {
      std::vector<char> buffer(n_bytes, 'A');
      buffer[n_bytes - 1] = 'B';
      int ierr            = MPI_Send_c(
        buffer.data(), n_bytes, MPI_CHAR, 1 /* dest */, 0 /* tag */, comm);
      CheckMPIFatal(ierr);
    }
  else if (myid == 1)
    {
      std::vector<char> buffer(n_bytes, '?');
      int               ierr = MPI_Recv_c(buffer.data(),
                            n_bytes,
                            MPI_CHAR,
                            0 /* src */,
                            0 /* tag */,
                            comm,
                            MPI_STATUS_IGNORE);
      CheckMPIFatal(ierr);

      if (buffer[0] != 'A' || buffer[n_bytes - 1] != 'B')
        {
          std::cerr << "MPI RECEIVE WAS INVALID:" << buffer[0]
                    << buffer[n_bytes - 1] << std::endl;
          MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

  if (myid == 0)
    std::cout << "TEST native-sendrecv: OK" << std::endl;
}

#else
void
test_send_recv()
{
  std::cout << "TEST native-sendrecv: NOT SUPPORTED" << std::endl;
}
#endif

int
main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int myid, ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &ranks);

  assert(ranks == 2);

  test_send_recv();

  MPI_Finalize();
  return 0;
}
