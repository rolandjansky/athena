# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthCUDAInterfaces )

# Component(s) in the package.
atlas_add_library( AthCUDAInterfacesLib
   AthCUDAInterfaces/*.h src/*.cxx
   OBJECT
   PUBLIC_HEADERS AthCUDAInterfaces
   LINK_LIBRARIES GaudiKernel )
set_target_properties( AthCUDAInterfacesLib PROPERTIES
   POSITION_INDEPENDENT_CODE TRUE )
