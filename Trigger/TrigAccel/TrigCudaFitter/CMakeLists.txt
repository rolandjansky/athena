################################################################################
# Package: TrigCudaFitter
################################################################################

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( TrigCudaFitter )

# As long as this CUDA code can't be interpreted by a C++ compiler as well,
# this package is not to be built.
if( NOT CMAKE_CUDA_COMPILER )
  message( STATUS "CUDA not found, TrigCudaFitter is not built" )
  return()
endif()

# External dependencies:
find_package( CUDAToolkit )

atlas_add_executable( trigCudaFitter
   src/*.cpp src/*.cu src/*.h src/*.cuh )

target_link_libraries( trigCudaFitter
   PRIVATE CUDA::cudart )
