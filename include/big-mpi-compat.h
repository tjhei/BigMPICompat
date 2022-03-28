/**
 */

#include <mpi.h>
#ifndef MPI_VERSION
#  error "Your MPI implementation does not define MPI_VERSION!"
#endif

#include <iostream>

#define CheckMPIFatal(ierr)                                        \
  if (ierr != MPI_SUCCESS)                                         \
    {                                                              \
      std::cerr << "MPI error " << ierr << " in line " << __LINE__ \
                << std::endl;                                      \
      MPI_Abort(MPI_COMM_WORLD, ierr);                             \
    }

#if MPI_VERSION < 3

#  error "We require at least MPI 3.0"

#endif

#if MPI_VERSION >= 4

// int MPI_File_write_ordered_c(MPI_File fh, const void *buf, MPI_Count count,
// MPI_Datatype datatype, MPI_Status *status)
//{
//}

#else

#endif

namespace BigMPICompat
{
  static constexpr MPI_Count mpi_max_signed_int = (1ULL << 31);
}

#if MPI_VERSION >= 4

#else

int
MPI_Type_contiguous_c(MPI_Count     count,
                      MPI_Datatype  oldtype,
                      MPI_Datatype *newtype)
{
  if (count <= BigMPICompat::mpi_max_signed_int)
    return MPI_Type_contiguous(count, oldtype, newtype);
  else
    {
      int             ierr;
      const MPI_Count max_signed_int = (1U << 31) - 1;

      MPI_Count size_old;
      ierr = MPI_Type_size_x(oldtype, &size_old);

      MPI_Count n_chunks          = count / max_signed_int;
      MPI_Count n_bytes_remainder = count % max_signed_int;

      MPI_Datatype chunks;
      ierr = MPI_Type_vector(
        n_chunks, max_signed_int, max_signed_int, oldtype, &chunks);
      if (ierr != MPI_SUCCESS)
        {
          std::cerr << "MPI_Type_vector() call failed!" << std::endl;
          return ierr;
        }

      MPI_Datatype remainder;
      ierr = MPI_Type_contiguous(n_bytes_remainder, oldtype, &remainder);
      if (ierr != MPI_SUCCESS)
        {
          std::cerr << "MPI_Type_contiguous() call failed!" << std::endl;
          return ierr;
        }

      int          blocklengths[2]  = {1, 1};
      MPI_Aint     displacements[2] = {0,
                                   static_cast<MPI_Aint>(n_chunks) * size_old *
                                     max_signed_int};
      MPI_Datatype types[2]         = {chunks, remainder};
      ierr =
        MPI_Type_create_struct(2, blocklengths, displacements, types, newtype);
      if (ierr != MPI_SUCCESS)
        {
          std::cerr << "MPI_Type_contiguous() call failed!" << std::endl;
          return ierr;
        }
      ierr = MPI_Type_commit(newtype);
      if (ierr != MPI_SUCCESS)
        {
          std::cerr << "MPI_Type_contiguous() call failed!" << std::endl;
          return ierr;
        }

      ierr = MPI_Type_free(&chunks);
      if (ierr != MPI_SUCCESS)
        {
          std::cerr << "MPI_Type_contiguous() call failed!" << std::endl;
          return ierr;
        }
      ierr = MPI_Type_free(&remainder);
      if (ierr != MPI_SUCCESS)
        {
          std::cerr << "MPI_Type_contiguous() call failed!" << std::endl;
          return ierr;
        }

#  ifndef MPI_COMPAT_SKIP_SIZE_CHECK
      MPI_Count size_new;
      ierr = MPI_Type_size_x(*newtype, &size_new);

      if (size_old * count != size_new)
        {
          std::cerr
            << "MPI_Type_contiguous_c() produced an invalid result. Expected = "
            << size_old << " * " << count << " = " << size_old * count
            << " but received " << size_new << std::endl;
          return MPI_ERR_INTERN;
        }
#  endif

      return MPI_SUCCESS;
    }
}

int
MPI_Send_c(const void * buf,
           MPI_Count    count,
           MPI_Datatype datatype,
           int          dest,
           int          tag,
           MPI_Comm     comm)
{
  if (count <= BigMPICompat::mpi_max_signed_int)
    return MPI_Send(buf, count, datatype, dest, tag, comm);

  MPI_Datatype bigtype;
  int          ierr;
  ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;

  ierr = MPI_Send(buf, 1, bigtype, dest, tag, comm);
  if (ierr != MPI_SUCCESS)
    return ierr;

  ierr = MPI_Type_free(&bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;
  return MPI_SUCCESS;
}

int
MPI_Recv_c(void *       buf,
           MPI_Count    count,
           MPI_Datatype datatype,
           int          source,
           int          tag,
           MPI_Comm     comm,
           MPI_Status * status)
{
  if (count <= BigMPICompat::mpi_max_signed_int)
    return MPI_Recv(buf, count, datatype, source, tag, comm, status);

  MPI_Datatype bigtype;
  int          ierr;
  ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;

  ierr = MPI_Recv(buf, 1, bigtype, source, tag, comm, status);
  if (ierr != MPI_SUCCESS)
    return ierr;

  ierr = MPI_Type_free(&bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;
  return MPI_SUCCESS;
}

#endif
