#include <big-mpi-compat.h>

#include <cassert>
#include <iostream>
#include <vector>


void
test_send_recv()
{
  MPI_Comm comm = MPI_COMM_WORLD;
  int      myid;
  MPI_Comm_rank(comm, &myid);

  const std::uint64_t n_bytes = (1ULL << 32) + 5;
  MPI_Datatype        bigtype;
  MPI_Type_contiguous_c(n_bytes, MPI_CHAR, &bigtype);
  MPI_Type_commit(&bigtype);

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

  MPI_Type_free(&bigtype);

  if (myid == 0)
    std::cout << "TEST big send_recv: OK" << std::endl;
}

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
