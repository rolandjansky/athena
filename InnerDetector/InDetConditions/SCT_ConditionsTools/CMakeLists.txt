# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_ConditionsTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( GTest )
find_package( GMock )

# Component(s) in the package:
atlas_add_library( SCT_ConditionsToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS SCT_ConditionsTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel GeoModelInterfaces Identifier InDetByteStreamErrors InDetConditionsSummaryService InDetReadoutGeometry RDBAccessSvcLib SCT_CablingLib SCT_ConditionsData
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GeoModelUtilities InDetIdentifier SiPropertiesToolLib StoreGateLib )

atlas_add_component ( SCT_ConditionsTools
                      src/components/*.cxx
                      LINK_LIBRARIES SCT_ConditionsToolsLib )

# Add unit tests
atlas_add_test( SCT_RODVetoTool_test
                SOURCES test/SCT_RODVetoTool_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
                LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} AthenaKernel GaudiKernel InDetIdentifier SCT_ConditionsToolsLib StoreGateLib TestTools
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( SCT_ConditionsConfig_test
                SCRIPT test/SCT_ConditionsConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/*.py )

