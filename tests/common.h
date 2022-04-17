#ifndef BIG_MPI_COMPAT_COMMON_H
#define BIG_MPI_COMPAT_COMMON_H

#include <cassert>
#include <iostream>
#include <vector>

#define CheckMPIFatal(ierr)                                                  \
  if (ierr != MPI_SUCCESS)                                                   \
    {                                                                        \
      std::cerr << "MPI error " << ierr << " in line " << __LINE__ << " in " \
                << __FILE__ << std::endl;                                    \
      MPI_Abort(MPI_COMM_WORLD, ierr);                                       \
    }



#endif
