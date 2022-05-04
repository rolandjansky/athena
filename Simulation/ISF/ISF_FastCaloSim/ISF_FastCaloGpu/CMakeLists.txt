# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( ISF_FastCaloGpu )

enable_language( CUDA )
find_package( CUDAToolkit )
#set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -gencode arch=compute_30,code=sm_30")
# Add a component library that has some CUDA code in it.
atlas_add_library( ISF_FastCaloGpuLib
  src/gpuQ.cu src/CaloGpuGeneral.cxx src/KernelWrapper.cu src/Rand4Hits.cu
  PUBLIC_HEADERS ISF_FastCaloGpu
  PRIVATE_LINK_LIBRARIES CUDA::cudart ${CUDA_curand_LIBRARY}
  )

