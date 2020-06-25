# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthCUDAServices )

# External(s).
find_package( TBB REQUIRED )

# Component(s) in the package.
atlas_add_component( AthCUDAServices
   src/*.hxx src/*.cxx src/*.cuh src/*.cu src/components/*.cxx
   INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
   LINK_LIBRARIES ${TBB_LIBRARIES} GaudiKernel AthCUDAInterfacesLib
   AthCUDACoreLib AthenaBaseComps )
