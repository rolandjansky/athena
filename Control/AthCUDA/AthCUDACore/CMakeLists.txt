# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthCUDACore )

# Necessary external(s).
find_package( TBB )
find_package( Threads )

# Component(s) in the package.
atlas_add_library( AthCUDACoreLib
   AthCUDACore/*.h AthCUDACore/*.cuh AthCUDACore/*.icc src/*.cu src/*.cxx
   SHARED
   PUBLIC_HEADERS AthCUDACore
   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
   LINK_LIBRARIES ${TBB_LIBRARIES} GaudiKernel
   PRIVATE_LINK_LIBRARIES Threads::Threads AthCUDAInterfacesLib )

# Unit test(s) in the package.
atlas_add_test( Memory
   SOURCES test/test_Memory.cu
   LINK_LIBRARIES AthCUDACoreLib )
atlas_add_test( Info
   SOURCES test/test_Info.cxx
   LINK_LIBRARIES AthCUDACoreLib )
