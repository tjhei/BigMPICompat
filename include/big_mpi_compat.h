/**
 *
 * BigMPICompat - a tiny MPI 4.x compatibility library
 *
 * Release under MIT at https://github.com/tjhei/BigMPICompat
 */
#ifndef BIG_MPI_COMPAT_H
#define BIG_MPI_COMPAT_H


#include <mpi.h>
#ifndef MPI_VERSION
#  error "Your MPI implementation does not define MPI_VERSION!"
#endif

#include <iostream>


#if MPI_VERSION < 3
#  error "BigMPICompat requires at least MPI 3.0"
#endif

/**
 * This namespace contains symbols related to the BigMPICompat library
 * to support large MPI routines on MPI implementations that implement
 * version 3.x of the standard.
 */
namespace BigMPICompat
{
  static constexpr MPI_Count mpi_max_count = (1ULL << 31);

} // namespace BigMPICompat

#if MPI_VERSION >= 4

#else

/**
 * Create a contiguous type of (possibly large) @p count.
 *
 * See the MPI 4.x standard for details.
 */
inline int
MPI_Type_contiguous_c(MPI_Count     count,
                      MPI_Datatype  oldtype,
                      MPI_Datatype *newtype)
{
  if (count <= BigMPICompat::mpi_max_count)
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

/**
 * Send a package to rank @p dest with a (possibly large) @p count.
 *
 * See the MPI 4.x standard for details.
 */
inline int
MPI_Send_c(const void * buf,
           MPI_Count    count,
           MPI_Datatype datatype,
           int          dest,
           int          tag,
           MPI_Comm     comm)
{
  if (count <= BigMPICompat::mpi_max_count)
    return MPI_Send(buf, count, datatype, dest, tag, comm);

  MPI_Datatype bigtype;
  int          ierr;
  ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;
  ierr = MPI_Type_commit(&bigtype);
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

/**
 * Receive a package from rank @p source with a (possibly large) @p count.
 *
 * See the MPI 4.x standard for details.
 */
inline int
MPI_Recv_c(void *       buf,
           MPI_Count    count,
           MPI_Datatype datatype,
           int          source,
           int          tag,
           MPI_Comm     comm,
           MPI_Status * status)
{
  if (count <= BigMPICompat::mpi_max_count)
    return MPI_Recv(buf, count, datatype, source, tag, comm, status);

  MPI_Datatype bigtype;
  int          ierr;
  ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;

  ierr = MPI_Type_commit(&bigtype);
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

/**
 * Broadcast a message of possibly large @p count of data from
 * the process with rank "root" to all other processes.
 *
 * See the MPI 4.x standard for details.
 */
inline int
MPI_Bcast_c(void *       buf,
            MPI_Count    count,
            MPI_Datatype datatype,
            unsigned int root_mpi_rank,
            MPI_Comm     comm)
{
  if (count <= BigMPICompat::mpi_max_count)
    return MPI_Bcast(buf, count, datatype, root_mpi_rank, comm);

  MPI_Datatype bigtype;
  int          ierr;
  ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;
  ierr = MPI_Type_commit(&bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;

  ierr = MPI_Bcast(buf, 1, bigtype, root_mpi_rank, comm);
  if (ierr != MPI_SUCCESS)
    return ierr;

  ierr = MPI_Type_free(&bigtype);
  if (ierr != MPI_SUCCESS)
    return ierr;
  return MPI_SUCCESS;
}
#endif


namespace BigMPICompat
{
  /**
   * Write a possibly large @p count of data at the location @p offset.
   *
   * See the MPI 4.x standard for details.
   */
  inline int
  MPI_File_write_at_c(MPI_File     fh,
                      MPI_Offset   offset,
                      const void * buf,
                      MPI_Count    count,
                      MPI_Datatype datatype,
                      MPI_Status * status)
  {
    if (count <= BigMPICompat::mpi_max_count)
      return MPI_File_write_at(fh, offset, buf, count, datatype, status);

    MPI_Datatype bigtype;
    int          ierr;
    ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    ierr = MPI_Type_commit(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_File_write_at(fh, offset, buf, 1, bigtype, status);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_Type_free(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    return MPI_SUCCESS;
  }

  /**
   * Collectively write a possibly large @p count of data at the
   * location @p offset.
   *
   * See the MPI 4.x standard for details.
   */
  inline int
  MPI_File_write_at_all_c(MPI_File     fh,
                          MPI_Offset   offset,
                          const void * buf,
                          MPI_Count    count,
                          MPI_Datatype datatype,
                          MPI_Status * status)
  {
    if (count <= BigMPICompat::mpi_max_count)
      return MPI_File_write_at_all(fh, offset, buf, count, datatype, status);

    MPI_Datatype bigtype;
    int          ierr;
    ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    ierr = MPI_Type_commit(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_File_write_at_all(fh, offset, buf, 1, bigtype, status);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_Type_free(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    return MPI_SUCCESS;
  }

  /**
   * Collectively write a possibly large @p count of data in order.
   *
   * See the MPI 4.x standard for details.
   */
  inline int
  MPI_File_write_ordered_c(MPI_File     fh,
                           const void * buf,
                           MPI_Count    count,
                           MPI_Datatype datatype,
                           MPI_Status * status)
  {
    if (count <= BigMPICompat::mpi_max_count)
      return MPI_File_write_ordered(fh, buf, count, datatype, status);

    MPI_Datatype bigtype;
    int          ierr;
    ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    ierr = MPI_Type_commit(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_File_write_ordered(fh, buf, 1, bigtype, status);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_Type_free(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    return MPI_SUCCESS;
  }

  /**
   * Read a possibly large @p count of data at the
   * location @p offset.
   *
   * See the MPI 4.x standard for details.
   */
  inline int
  MPI_File_read_at_c(MPI_File     fh,
                     MPI_Offset   offset,
                     void *       buf,
                     MPI_Count    count,
                     MPI_Datatype datatype,
                     MPI_Status * status)
  {
    if (count <= BigMPICompat::mpi_max_count)
      return MPI_File_read_at(fh, offset, buf, count, datatype, status);

    MPI_Datatype bigtype;
    int          ierr;
    ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    ierr = MPI_Type_commit(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_File_read_at(fh, offset, buf, 1, bigtype, status);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_Type_free(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    return MPI_SUCCESS;
  }

  /**
   * Collectively read a possibly large @p count of data at the
   * location @p offset.
   *
   * See the MPI 4.x standard for details.
   */
  inline int
  MPI_File_read_at_all_c(MPI_File     fh,
                         MPI_Offset   offset,
                         void *       buf,
                         MPI_Count    count,
                         MPI_Datatype datatype,
                         MPI_Status * status)
  {
    if (count <= BigMPICompat::mpi_max_count)
      return MPI_File_read_at_all(fh, offset, buf, count, datatype, status);

    MPI_Datatype bigtype;
    int          ierr;
    ierr = MPI_Type_contiguous_c(count, datatype, &bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    ierr = MPI_Type_commit(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_File_read_at_all(fh, offset, buf, 1, bigtype, status);
    if (ierr != MPI_SUCCESS)
      return ierr;

    ierr = MPI_Type_free(&bigtype);
    if (ierr != MPI_SUCCESS)
      return ierr;
    return MPI_SUCCESS;
  }

} // namespace BigMPICompat

#endif
