# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetMomentTools )

# External dependencies:
find_package( Boost )
find_package( FastJet )
find_package( ROOT COMPONENTS Core Hist RIO TMVA)
find_package( lwtnn )
find_package( nlohmann_json )


# Component(s) in the package:
atlas_add_library( JetMomentToolsLib
   JetMomentTools/*.h Root/*.cxx
   PUBLIC_HEADERS JetMomentTools
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} AsgDataHandlesLib AsgTools InDetTrackSelectionToolLib JetCalibToolsLib JetEDM JetInterface JetRecLib JetUtils PFlowUtilsLib TrackVertexAssociationToolLib xAODCaloEvent xAODEventInfo xAODJet xAODMissingET xAODTracking xAODTruth
   PRIVATE_LINK_LIBRARIES CaloGeoHelpers FourMomUtils xAODPFlow CxxUtils PathResolver nlohmann_json::nlohmann_json
   )

if( NOT XAOD_STANDALONE )
   set( extra_libs )
   if( NOT XAOD_ANALYSIS )
      set( extra_libs JetRecCaloLib CaloDetDescrLib CaloEvent CaloIdentifier AthenaKernel StoreGateLib )
   endif()
   atlas_add_component( JetMomentTools
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODJet GaudiKernel JetCalibToolsLib JetEDM JetInterface JetRecLib JetUtils PFlowUtilsLib PathResolver JetMomentToolsLib ${extra_libs} )
endif()

atlas_add_dictionary( JetMomentToolsDict
	Root/JetMomentToolsDict.h
	Root/selection.xml
	LINK_LIBRARIES JetMomentToolsLib PFlowUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_data( share/*.root )

# Compile executables
atlas_add_executable( JetJVTNN_testCfg.exe
   util/JetJvtNN_testCfg.cxx
   INCLUDE_DIRS ${LWTNN_INCLUDE_DIRS}
   LINK_LIBRARIES
   ${LWTNN_LIBRARIES}
   PathResolver
   nlohmann_json::nlohmann_json
)

atlas_add_test( JetJVTNN_TestCfg
   SCRIPT JetJVTNN_testCfg.exe
   POST_EXEC_SCRIPT nopost.sh )
