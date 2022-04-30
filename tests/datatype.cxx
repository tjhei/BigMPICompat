#include <big_mpi_compat.h>

#include "common.h"

void
test_create_data_type(const std::uint64_t n_bytes, int myrank)
{
  MPI_Datatype bigtype;
  int ierr = BigMPICompat::MPI_Type_contiguous_c(n_bytes, MPI_CHAR, &bigtype);
  if (ierr != MPI_SUCCESS)
    {
      std::cout
        << "MPI_Type_contiguous_c failed, probably because your MPI implementation is broken!"
        << std::endl;
    }
  CheckMPIFatal(ierr);
  ierr = MPI_Type_commit(&bigtype);
  CheckMPIFatal(ierr);

  if (myrank == 0)
    std::cout << "Test creating big data type: n_bytes=" << n_bytes;

  int size32;
  ierr = MPI_Type_size(bigtype, &size32);
  CheckMPIFatal(ierr);

  if (myrank == 0)
    {
      if (size32 == MPI_UNDEFINED)
        std::cout << " size32=UNDEFINED (too big)";
      else
        std::cout << " size32=" << size32;

      assert(size32 == MPI_UNDEFINED);
    }

  MPI_Count size64 = -1;
  //#ifdef WITH_MPI3
  ierr = MPI_Type_size_x(bigtype, &size64);
  CheckMPIFatal(ierr);
  //  #else
  //  #error "ohoh"
  //#endif

  if (myrank == 0)
    std::cout << " size64=" << size64 << " ";

  assert(size64 == static_cast<MPI_Count>(n_bytes));

  MPI_Type_free(&bigtype);

  if (myrank == 0)
    std::cout << "OK" << std::endl;
}

int
main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  test_create_data_type(1ULL << 31, 0);
  test_create_data_type(1ULL << 32, 0);
  test_create_data_type(1ULL << 33, 0);

  MPI_Finalize();
  return 0;
}
