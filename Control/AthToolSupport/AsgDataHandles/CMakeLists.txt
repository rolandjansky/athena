# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AsgDataHandles )

if( NOT XAOD_STANDALONE )

# Component(s) in the package:
atlas_add_library( AsgDataHandlesLib INTERFACE
   AsgDataHandles/*.h AsgDataHandles/*.icc
   PUBLIC_HEADERS AsgDataHandles
   PRIVATE_INCLUDE_DIRS
   LINK_LIBRARIES StoreGateLib )

else()

# Component(s) in the package:
atlas_add_library( AsgDataHandlesLib
   AsgDataHandles/*.h AsgDataHandles/*.icc Root/*.cxx
   PUBLIC_HEADERS AsgDataHandles
   PRIVATE_INCLUDE_DIRS
   LINK_LIBRARIES AsgMessagingLib xAODRootAccessInterfaces )

endif()
