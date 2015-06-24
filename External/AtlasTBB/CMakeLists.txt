cmake_minimum_required(VERSION 2.8)

include(ExternalProject)

set(PROJECT AtlasTBB)
#set(VERSION 43_20150424oss)
set(VERSION 42_20131003oss)

message("install ${PROJECT} to ${CMAKE_INSTALL_PROJECT}")

ExternalProject_Add(${PROJECT}
  URL ${CMAKE_CURRENT_SOURCE_DIR}/src/tbb${VERSION}_src.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}
  CONFIGURE_COMMAND true
  BUILD_IN_SOURCE 1
  set(tbb_build_options CPLUS=${CMAKE_CXX_COMPILER} CONLY=${CMAKE_C_COMPILER})
  COMMAND make ${tbb_build_options}
  INSTALL_COMMAND ${CMAKE_COMMAND} -Dinstall_dir=<INSTALL_DIR> -Dsource_dir=<SOURCE_DIR> -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallTBB.cmake
)


