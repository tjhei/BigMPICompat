#include <mpi.h>

#include <iostream>

int
main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int ver, subver;
  MPI_Get_version(&ver, &subver);

  std::cout << "MPI version " << ver << "." << subver << std::endl;

#ifdef MPI_MAX_LIBRARY_VERSION_STRING
  int  len;
  char mpi_lib_ver[MPI_MAX_LIBRARY_VERSION_STRING];

  MPI_Get_library_version(mpi_lib_ver, &len);
  std::cout << "MPI library version " << mpi_lib_ver << std::endl;
#endif

#ifdef MPICH2_VERSION
  std::cout << "MPICH2_VERSION " << MPICH2_VERSION << std::endl;
#endif
#ifdef OMPI_MAJOR_VERSION
  std::cout << "OMPI version " << OMPI_MAJOR_VERSION << "."
            << OMPI_MINOR_VERSION << "." << OMPI_RELEASE_VERSION << std::endl;
#endif

  MPI_Finalize();
  return 0;
}
