# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( ISF_FastCaloGpu )

# As long as this CUDA code can't be interpreted by a C++ compiler as well,
# this package is not to be built.
if( NOT CMAKE_CUDA_COMPILER )
  message( STATUS "CUDA not found, ISF_FastCaloGpu is not built" )
  return()
endif()


find_package( CUDAToolkit )
# Add a component library that has some CUDA code in it.
atlas_add_library( ISF_FastCaloGpuLib
  src/gpuQ.cu src/CaloGpuGeneral.cxx src/KernelWrapper.cu src/Rand4Hits.cu
  PUBLIC_HEADERS ISF_FastCaloGpu
  PRIVATE_LINK_LIBRARIES CUDA::cudart ${CUDA_curand_LIBRARY}
  )

