# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetUtils )

# Component(s) in the package:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_library( JetUtils
      JetUtils/*.h JetUtils/*.icc Root/*.cxx
      PUBLIC_HEADERS JetUtils
      LINK_LIBRARIES xAODCaloEvent xAODJet xAODTracking
      PRIVATE_LINK_LIBRARIES CaloGeoHelpers xAODPFlow )
else()
   atlas_add_library( JetUtils
      JetUtils/*.h JetUtils/*.icc Root/*.cxx src/*.cxx
      PUBLIC_HEADERS JetUtils
      LINK_LIBRARIES CaloEvent xAODCaloEvent xAODJet xAODTracking
      PRIVATE_LINK_LIBRARIES CaloGeoHelpers xAODPFlow TileEvent )
endif()

# Test(s) in the package:
if( NOT XAOD_STANDALONE )
   atlas_add_test( JetUtils_CaloQual_test
      SOURCES test/JetUtils_CaloQual_test.cxx
      LINK_LIBRARIES xAODCaloEvent JetUtils SGTools AthenaKernel
      LOG_IGNORE_PATTERN "no dictionary for class" )
elseif( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_test( JetUtils_CaloQual_test
      SOURCES test/JetUtils_CaloQual_test.cxx
      LINK_LIBRARIES xAODRootAccess xAODCaloEvent JetUtils
      LOG_IGNORE_PATTERN "no dictionary for class|xAOD::Init" )
endif()

