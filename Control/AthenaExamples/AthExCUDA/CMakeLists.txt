# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# This package needs CUDA to be handled as a "first class language" by CMake.
cmake_minimum_required( VERSION 3.10 )

# Set the name of the package.
atlas_subdir( AthExCUDA )

# The build of this package needs CUDA. If it's not available, don't try to do
# anything...
include( CheckLanguage )
check_language( CUDA )
if( NOT CMAKE_CUDA_COMPILER )
  message( STATUS "CUDA not found, AthExCUDA is not built" )
  return()
endif()
enable_language( CUDA )

# Add a component library that has some CUDA code in it.
atlas_add_component( AthExCUDA
   src/*.h src/*.cxx src/*.cu src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel )

# Install extra files from the package.
atlas_install_joboptions( share/*.py )
