#Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloUtils )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CaloUtilsLib CaloUtils/*.h CaloUtils/*.icc src/*.h src/*.cxx
   PUBLIC_HEADERS CaloUtils
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES CaloConditions CaloEvent CaloGeoHelpers CaloIdentifier
   AthenaBaseComps AthenaKernel Navigation FourMom xAODCaloEvent xAODCore GaudiKernel
   CaloDetDescrLib StoreGateLib CaloInterfaceLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils
   SGTools IdDictParser Identifier )

atlas_add_component( CaloUtils src/components/*.cxx
   LINK_LIBRARIES CaloUtilsLib )

atlas_add_dictionary( CaloUtilsDict
   CaloUtils/CaloUtilsDict.h CaloUtils/selection.xml
   LINK_LIBRARIES CaloUtilsLib )

# Helper variable:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Test(s) in the package:
atlas_add_test( CaloLayerCalculator_test
   SOURCES test/CaloLayerCalculator_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier
   StoreGateLib xAODCaloEvent GaudiKernel TestTools IdDictParser CaloUtilsLib
   LOG_IGNORE_PATTERN "^lar decode|mask/zero|initialize_from_dict|^ channel range|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( CaloVertexedCell_test
   SOURCES test/CaloVertexedCell_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier
   StoreGateLib GaudiKernel TestTools IdDictParser CaloUtilsLib
   LOG_IGNORE_PATTERN "^lar decode|mask/zero|initialize_from_dict|^ channel range|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( CaloVertexedCluster_test
   SOURCES test/CaloVertexedCluster_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDetDescrLib CaloIdentifier StoreGateLib
   xAODCaloEvent GaudiKernel TestTools IdDictParser CaloUtilsLib
   LOG_IGNORE_PATTERN "^lar decode|mask/zero|initialize_from_dict|^ channel range|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( exceptions_test
   SOURCES test/exceptions_test.cxx
   LINK_LIBRARIES CaloUtilsLib )

atlas_add_test( CaloTowerStore_test
   SCRIPT test/CaloTowerStore_test.sh
   LOG_IGNORE_PATTERN "Reading file|Unable to locate catalog|Cache alignment"
   PROPERTIES TIMEOUT 500 )

atlas_add_test( CaloTowerBuilderTool_test
   SCRIPT test/CaloTowerBuilderTool_test.sh
   LOG_IGNORE_PATTERN "Reading file|Unable to locate catalog|Cache alignment"
   ENVIRONMENT "ATLAS_REFERENCE_TAG=CaloUtils/CaloUtils-01-00-13"
   PROPERTIES TIMEOUT 500 )

atlas_add_test( ToolWithConstants_test
   SCRIPT test/ToolWithConstants_test.py
   LOG_IGNORE_PATTERN "no dictionary for|by peeking into|Current filenames:" )

# Install files from the package:
atlas_install_joboptions( share/*.txt share/*.py )
