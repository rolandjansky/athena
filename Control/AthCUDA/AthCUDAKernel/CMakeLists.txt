# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthCUDAKernel )

# Component(s) in the package.
atlas_add_library( AthCUDAKernelLib
   AthCUDAKernel/*.h AthCUDAKernel/*.cuh src/*.cxx
   OBJECT
   PUBLIC_HEADERS AthCUDAKernel
   LINK_LIBRARIES GaudiKernel AthCUDAInterfacesLib AthCUDACoreLib )
set_target_properties( AthCUDAKernelLib PROPERTIES
   POSITION_INDEPENDENT_CODE ON )

# Test(s) in the package.
atlas_add_test( ArrayKernelTask
   SOURCES test/test_ArrayKernelTask.cu
   LINK_LIBRARIES AthCUDAKernelLib )
