################################################################################
# Package: TrigInDetCUDA
################################################################################

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( TrigInDetCUDA )

# As long as this CUDA code can't be interpreted by a C++ compiler as well,
# this package is not to be built.
if( NOT CMAKE_CUDA_COMPILER )
  message( STATUS "CUDA not found, TrigInDetCUDA is not built" )
  return()
endif()

# External dependencies:
find_package( TBB )

# Add a component library that has some CUDA code in it.
atlas_add_component( TrigInDetCUDA
   src/*.h src/*.cuh src/*.cu
   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
   LINK_LIBRARIES TrigAccelEvent ${TBB_LIBRARIES})

 
