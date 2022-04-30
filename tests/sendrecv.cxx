#include <big_mpi_compat.h>

#include "common.h"


void
test_send_recv_manual()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int      myid;
  MPI_Comm_rank(comm, &myid);

  const std::uint64_t n_bytes = (1ULL << 32) + 5;
  MPI_Datatype        bigtype;
  int                 ierr = MPI_Type_contiguous_c(n_bytes, MPI_CHAR, &bigtype);
  CheckMPIFatal(ierr);
  ierr = MPI_Type_commit(&bigtype);
  CheckMPIFatal(ierr);

  if (myid == 0)
    {
      std::vector<char> buffer(n_bytes, 'A');
      buffer[n_bytes - 1] = 'B';
      int ierr =
        MPI_Send(buffer.data(), 1, bigtype, 1 /* dest */, 0 /* tag */, comm);
      CheckMPIFatal(ierr);
    }
  else if (myid == 1)
    {
      std::vector<char> buffer(n_bytes, '?');
      int               ierr = MPI_Recv(buffer.data(),
                          1,
                          bigtype,
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

  ierr = MPI_Type_free(&bigtype);
  CheckMPIFatal(ierr);

  if (myid == 0)
    std::cout << "TEST send_recv_manual: OK" << std::endl;
}

void
test_send_and_recv()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int      myid;
  MPI_Comm_rank(comm, &myid);

  const std::uint64_t count = (1ULL << 32) + 5;

  if (myid == 0)
    {
      std::vector<short> buffer(count, 0);
      buffer[count - 1] = 2;
      int ierr          = MPI_Send_c(
        buffer.data(), count, MPI_SHORT, 1 /* dest */, 0 /* tag */, comm);
      CheckMPIFatal(ierr);
    }
  else if (myid == 1)
    {
      std::vector<short> buffer(count, 42);
      int                ierr = MPI_Recv_c(buffer.data(),
                            count,
                            MPI_SHORT,
                            0 /* src */,
                            0 /* tag */,
                            comm,
                            MPI_STATUS_IGNORE);
      CheckMPIFatal(ierr);

      if (buffer[0] != 0 || buffer[count - 1] != 2)
        {
          std::cerr << "MPI RECEIVE WAS INVALID:" << buffer[0] << ' '
                    << buffer[count - 1] << std::endl;
          MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

  if (myid == 0)
    std::cout << "TEST send_and_recv: OK" << std::endl;
}

int
main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int myid, ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &ranks);

  assert(ranks == 2);

  test_send_recv_manual();
  test_send_and_recv();

  MPI_Finalize();
  return 0;
}
