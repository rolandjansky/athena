cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

set(PROJECT External/AtlasBlas)
set(VERSION 20070405)

message(STATUS "install ${PROJECT} to ${CMAKE_INSTALL_PREFIX}")
message(STATUS "install ${PROJECT} to ${PROJECT_INSTALL_DIR}")

ExternalProject_Add(AtlasBlas
  URL ${CMAKE_CURRENT_SOURCE_DIR}/src/blas-${VERSION}.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  INSTALL_COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib && cp ${CMAKE_CURRENT_BINARY_DIR}/src/AtlasBlas/libblas.so.3.0.3 ${CMAKE_INSTALL_PREFIX}/lib/libblas.so
  CONFIGURE_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/make.inc ${CMAKE_CURRENT_BINARY_DIR}/src/AtlasBlas
  BUILD_COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/src/AtlasBlas && make -f ${CMAKE_CURRENT_BINARY_DIR}/src/AtlasBlas/Makefile FORTRAN=${CMAKE_Fortran_COMPILER} OPTS=-fpic
)
