# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthExSYCL )

# The target_link_options(...) call requires at least CMake 3.13.
cmake_minimum_required( VERSION 3.13 )

# Expose FindSYCL.cmake to CMake, and use it to figure out whether we can
# compile SYCL code in the current environment.
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
find_package( SYCL )
if( NOT SYCL_FOUND )
   return()
endif()

# Add a component library that has some SYCL code in it.
atlas_add_component( AthExSYCL
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel )
target_compile_options( AthExSYCL PRIVATE ${SYCL_FLAGS} )
target_link_options( AthExSYCL PRIVATE ${SYCL_FLAGS} )

# Install extra files from the package.
atlas_install_joboptions( share/*.py )
