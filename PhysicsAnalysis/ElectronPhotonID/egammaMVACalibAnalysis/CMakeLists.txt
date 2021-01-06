# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaMVACalibAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Tree TreePlayer TMVA XMLIO Core MathCore Hist RIO )

# Component(s) in the package:
atlas_add_library( egammaMVACalibAnalysisLib
   egammaMVACalibAnalysis/*.h Root/*.cxx
   PUBLIC_HEADERS egammaMVACalibAnalysis
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTools xAODCaloEvent xAODEgamma
   xAODTracking egammaMVACalibLib
   PRIVATE_LINK_LIBRARIES PathResolver MVAUtils )

if( NOT XAOD_STANDALONE )
   atlas_add_component( egammaMVACalibAnalysis
      src/*.cxx src/components/*.cxx
      LINK_LIBRARIES egammaMVACalibAnalysisLib)
endif()

atlas_add_dictionary( egammaMVACalibAnalysisDict
	egammaMVACalibAnalysis/egammaMVACalibAnalysisDict.h
	egammaMVACalibAnalysis/selection.xml
	LINK_LIBRARIES egammaMVACalibAnalysisLib )

atlas_add_executable( egammaMVACalibAnalysis_mem_test_initialize
   util/mem_test_initialize.cxx
   LINK_LIBRARIES egammaMVACalibAnalysisLib )

if( XAOD_STANDALONE )
   atlas_add_executable( testClusterMVACalib
      util/testClusterMVACalib.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo xAODEgamma xAODCaloEvent
      xAODCore AsgMessagingLib egammaMVACalibAnalysisLib )
endif()

# Test(s) in the package:
# Not working now because egammaMVACalibDict was removed.
#atlas_add_test( mem_test SCRIPT test/mem_test.py
#                PROPERTIES TIMEOUT 300
#                LOG_IGNORE_PATTERN " GB$" )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

if (NOT XAOD_STANDALONE AND NOT XAOD_ANALYSIS)
  atlas_add_test( flake8_test
                  SCRIPT ${ATLAS_FLAKE8} ${CMAKE_CURRENT_SOURCE_DIR}/test
                  POST_EXEC_SCRIPT nopost.sh )
endif ()
