#include <big-mpi-compat.h>

#include <cassert>
#include <iostream>
#include <vector>

void
test_write_at(const std::uint64_t n_bytes)
{
  int ierr;
  int myid, ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &ranks);
  assert(ranks == 2);

  std::uint64_t offset = myid * n_bytes;

  MPI_File fh;
  MPI_Info info;
  ierr = MPI_Info_create(&info);
  CheckMPIFatal(ierr);

  ierr = MPI_File_open(MPI_COMM_WORLD,
                       "native-write-at.data",
                       MPI_MODE_CREATE //| MPI_MODE_DELETE_ON_CLOSE
                         | MPI_MODE_WRONLY,
                       info,
                       &fh);
  CheckMPIFatal(ierr);

  std::vector<char> buffer(n_bytes, '?');
  buffer[0] = 'A' + myid;
  ierr      = BigMPICompat::MPI_File_write_at_c(
    fh, offset, buffer.data(), buffer.size(), MPI_CHAR, MPI_STATUS_IGNORE);
  // CheckMPIFatal(ierr);

  ierr = MPI_File_sync(fh);
  CheckMPIFatal(ierr);

  MPI_Barrier(MPI_COMM_WORLD);

  if (myid == 0)
    {
      int result = system(
        "md5sum native-write-at.data | grep 4c1fe47ac9c80d7af0ea289c90e13132");
      if (result == 0)
        std::cout << "native-io: md5sum: OK" << std::endl;
      else
        {
          std::cerr << "native-io: md5sum FAILED - result: " << result
                    << std::endl;
          // MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

  ierr = MPI_File_close(&fh);
  CheckMPIFatal(ierr);
}

int
main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  test_write_at((1ULL << 32) + 2);

  MPI_Finalize();
  return 0;
}
