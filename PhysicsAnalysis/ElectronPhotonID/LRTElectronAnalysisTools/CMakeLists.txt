# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LRTElectronAnalysisTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO Hist Physics )

# Libraries in the package:
atlas_add_library( LRTElectronAnalysisToolsLib
   LRTElectronAnalysisTools/*.h Root/*.cxx
   PUBLIC_HEADERS LRTElectronAnalysisTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo EgammaAnalysisInterfacesLib PATInterfaces AsgDataHandlesLib
   PRIVATE_LINK_LIBRARIES FourMomUtils xAODTrigger PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( LRTElectronAnalysisTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgAnalysisInterfaces AthContainers AthenaBaseComps FourMomUtils GaudiKernel LRTElectronAnalysisToolsLib StoreGateLib xAODJet )
endif()

atlas_add_dictionary( LRTElectronAnalysisToolsDict
   LRTElectronAnalysisTools/LRTElectronAnalysisToolsDict.h
   LRTElectronAnalysisTools/selection.xml
   LINK_LIBRARIES LRTElectronAnalysisToolsLib )