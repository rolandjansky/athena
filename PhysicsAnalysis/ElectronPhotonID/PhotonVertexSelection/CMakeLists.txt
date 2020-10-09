# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PhotonVertexSelection )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO Physics TMVA )

# Libraries in the package:
atlas_add_library( PhotonVertexSelectionLib
   PhotonVertexSelection/*.h Root/*.cxx
   PUBLIC_HEADERS PhotonVertexSelection
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo
   xAODTracking
   PRIVATE_LINK_LIBRARIES IsolationCorrectionsLib PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PhotonVertexSelection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODEgamma xAODTracking AthContainers AthenaBaseComps
      GaudiKernel PhotonVertexSelectionLib )
endif()

atlas_add_dictionary( PhotonVertexSelectionDict
   PhotonVertexSelection/PhotonVertexSelectionDict.h
   PhotonVertexSelection/selection.xml
   LINK_LIBRARIES PhotonVertexSelectionLib )

# Test(s) in the package:
atlas_add_test( PhotonVertexSelection_test
   SCRIPT test/testPhotonVertexSelection.py )
