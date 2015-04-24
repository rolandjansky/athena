cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

set(PROJECT External/AtlasEigen)
set(VERSION 3.1.3)


message(STATUS "MESSAGE: install ${PROJECT} to ${CMAKE_INSTALL_PREFIX}")

ExternalProject_Add(AtlasEigen
  URL ${CMAKE_CURRENT_SOURCE_DIR}/src/eigen-${VERSION}.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}
  PATCH_COMMAND patch -p0 < ${CMAKE_CURRENT_SOURCE_DIR}/cmt/placement-new.patch 
             && patch -p0 < ${CMAKE_CURRENT_SOURCE_DIR}/cmt/typedef-warnings.patch
   
  INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DFORTRAN=${CMAKE_Fortran_COMPILER}
)


