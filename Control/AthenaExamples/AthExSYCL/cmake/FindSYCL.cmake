# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This module is used to find out whether the C++ compiler set up to build the
# project is able to compile SYCL code. If so, it provides helper variables
# to the project configuration to set up the build of SYCL
# libraries/executables.
#

# We use CMake's built-in modules to look for SYCL capabilities.
include( CheckIncludeFileCXX )
include( CheckCXXSourceCompiles )
set( CMAKE_REQUIRED_QUIET TRUE )

# Greet the user.
if( NOT SYCL_FIND_QUIETLY )
   message( STATUS "Checking if ${CMAKE_CXX_COMPILER} is SYCL capable..." )
endif()

# If the compiler is not Clang, then don't even try anything more. Since for
# now we're only seriously experimenting with the Intel LLVM/Clang code.
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )

   # Check if it's possible to use the <CL/sycl.hpp> header using the version
   # of Clang given to CMake.
   set( CMAKE_REQUIRED_FLAGS -fsycl )
   check_include_file_cxx( "CL/sycl.hpp" SYCL_FOUND )

endif()

# Set up the variables that the project should use.
if( SYCL_FOUND )
   if( NOT SYCL_FIND_QUIETLY )
      message( STATUS
         "Checking if ${CMAKE_CXX_COMPILER} is SYCL capable... success" )
      message( STATUS "Checking for available SYCL target(s)..." )
   endif()
   set( SYCL_FLAGS "-fsycl" )
   # Figure out which SYCL target platforms are available.
   set( SYCL_TARGETS )
   foreach( _target "nvptx64-*-*-sycldevice" "spir64-*-*-sycldevice" )
      set( CMAKE_REQUIRED_FLAGS "-fsycl -fsycl-targets=${_target}" )
      check_cxx_source_compiles( "
         #include <CL/sycl.hpp>
         int main() {
            cl::sycl::platform::get_platforms();
            return 0;
         }
         " _syclTarget${_target}Found )
      if( _syclTarget${_target}Found )
         if( NOT SYCL_FIND_QUIETLY )
            message( STATUS "  - Found target: ${_target}" )
         endif()
         list( APPEND SYCL_TARGETS ${_target} )
      endif()
      unset( _syclTarget${_target}Found )
   endforeach()
   if( NOT "${SYCL_TARGETS}" STREQUAL "" )
      string( REPLACE ";" "," _targets "${SYCL_TARGETS}" )
      list( APPEND SYCL_FLAGS "-fsycl-targets=${_targets}" )
      unset( _targets )
   endif()
else()
   if( NOT SYCL_FIND_QUIETLY )
      message( STATUS
         "Checking if ${CMAKE_CXX_COMPILER} is SYCL capable... failure" )
   endif()
endif()
