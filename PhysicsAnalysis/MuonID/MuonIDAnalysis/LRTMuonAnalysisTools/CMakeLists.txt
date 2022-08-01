# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LRTMuonAnalysisTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO Hist Physics )

# Libraries in the package:
atlas_add_library( LRTMuonAnalysisToolsLib
   LRTMuonAnalysisTools/*.h Root/*.cxx
   PUBLIC_HEADERS LRTMuonAnalysisTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODMuon xAODEventInfo MuonAnalysisInterfacesLib PATInterfaces AsgDataHandlesLib
   PRIVATE_LINK_LIBRARIES FourMomUtils xAODTrigger PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( LRTMuonAnalysisTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgAnalysisInterfaces AthContainers AthenaBaseComps FourMomUtils GaudiKernel LRTMuonAnalysisToolsLib StoreGateLib xAODJet )
endif()

atlas_add_dictionary( LRTMuonAnalysisToolsDict
   LRTMuonAnalysisTools/LRTMuonAnalysisToolsDict.h
   LRTMuonAnalysisTools/selection.xml
   LINK_LIBRARIES LRTMuonAnalysisToolsLib )
