
cmake_minimum_required(VERSION 3.0)
Project(bigmpicompat)


find_package(MPI REQUIRED)

message(STATUS "Found MPI version ${MPI_CXX_VERSION_MAJOR}.${MPI_CXX_VERSION_MINOR}")

# tests
SET(TESTS "tests/datatype.cxx" "tests/sendrecv.cxx")
SET(BINARIES "")

foreach(TARGET_SRC ${TESTS})
  get_filename_component(TARGET ${TARGET_SRC} NAME_WLE)
  message(STATUS "processing ${TARGET} from ${TARGET_SRC}...")
  add_executable(${TARGET} ${TARGET_SRC})
  list(APPEND BINARIES ${TARGET})
  target_include_directories(${TARGET} PRIVATE ${MPI_CXX_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/include")
  target_compile_options(${TARGET} PRIVATE ${MPI_CXX_COMPILE_FLAGS} "-Wall" "-O2")
  target_link_libraries(${TARGET} ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} "-Wall" "-O2")
endforeach()

# mpiinfo
set(TARGET "mpiinfo")
add_executable(${TARGET} source/mpiinfo.cxx)
list(APPEND BINARIES ${TARGET})
target_include_directories(${TARGET} PRIVATE ${MPI_CXX_INCLUDE_PATH} "${CMAKE_SOURCE_DIR}/include")
target_compile_options(${TARGET} PRIVATE ${MPI_CXX_COMPILE_FLAGS} "-Wall" "-O2")
target_link_libraries(${TARGET} ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS} "-Wall" "-O2")


# run all tests
ADD_CUSTOM_TARGET(run
  DEPENDS ${BINARIES}
  VERBATIM
  COMMAND ./mpiinfo
  COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
  COMMAND ./datatype
  COMMAND ${CMAKE_COMMAND} -E echo "test ok"
  COMMAND mpirun -n 2 ./sendrecv
  COMMAND ${CMAKE_COMMAND} -E echo "test ok"
  COMMAND echo "all good!"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
