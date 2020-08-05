# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileConditions )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TileConditionsLib
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileConditions
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier GaudiKernel TileCalibBlobObjs TileIdentifier StoreGateLib SGtests CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TileDetDescr PathResolver )

atlas_add_component( TileConditions
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES TileConditionsLib )

atlas_add_dictionary( TileConditionsDict
                      TileConditions/TileConditionsDict.h
                      TileConditions/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel TileCalibBlobObjs TileIdentifier CaloDetDescrLib TileDetDescr PathResolver TileConditionsLib )

# Test(s) in the package:
atlas_add_test( TileCondToolTMDB_test
                SOURCES test/TileCondToolTMDB_test.cxx
                LINK_LIBRARIES TileConditionsLib TileCalibBlobObjs AthenaBaseComps TestTools GaudiKernel ${CORAL_LIBRARIES} )

atlas_add_test( TileCablingService_test
                SOURCES test/TileCablingService_test.cxx
                LINK_LIBRARIES TileConditionsLib TileIdentifier StoreGateLib IdDictParser TestTools CxxUtils )

atlas_add_test( TileCablingService_UpgradeABC_test
                SOURCES test/TileCablingService_UpgradeABC_test.cxx
                LINK_LIBRARIES TileConditionsLib TileIdentifier StoreGateLib IdDictParser TestTools CxxUtils )

atlas_add_test( TileConditionTools_test
                SOURCES test/TileConditionTools_test.cxx
                LINK_LIBRARIES TileConditionsLib TileCalibBlobObjs AthenaBaseComps StoreGateLib GaudiKernel TestTools ${CORAL_LIBRARIES} )

atlas_add_test( TileEMScaleComponents_test
                SOURCES test/TileEMScaleComponents_test.cxx
                LINK_LIBRARIES TileConditionsLib TileIdentifier TileCalibBlobObjs AthenaBaseComps StoreGateLib GaudiKernel IdDictParser TestTools ${CORAL_LIBRARIES} )
# Needed for the plugin service to see the test components
# defined in the test binary.
set_target_properties( TileConditions_TileEMScaleComponents_test PROPERTIES ENABLE_EXPORTS True )

atlas_add_test( TileNoiseTools_test
                SOURCES test/TileNoiseTools_test.cxx
                LINK_LIBRARIES TileConditionsLib TileCalibBlobObjs AthenaBaseComps StoreGateLib GaudiKernel TestTools ${CORAL_LIBRARIES} )

atlas_add_test( TileDCSComponents_test
                SOURCES test/TileDCSComponents_test.cxx
                LINK_LIBRARIES TileConditionsLib AthenaBaseComps StoreGateLib GaudiKernel IdDictParser TestTools ${CORAL_LIBRARIES} )


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/Tile2003.* share/Tile2004.* share/TileDefault.* share/TileNo*.* share/dleak*.dat share/dsleak*.dat share/leak*.dat share/dpulse*.dat share/pulse*.dat share/noise*.dat share/ai_lo*.dat share/ai_hi*.dat share/bi_lo*.dat share/bi_hi*.dat share/ci_lo*.dat share/ci_hi*.dat share/DCS*.dat share/BadHVChannels.dat share/wiener*.dat)

if( NOT SIMULATIONBASE )
    # Configuration tests
    atlas_add_test( TileCondConfigTest
    		    SCRIPT python -m TileConditions.TileConditionsConfig
                    POST_EXEC_SCRIPT nopost.sh )


    atlas_add_test( TileFolders_test
                   SCRIPT python -m unittest -v TileConditions.TileFolders
                   POST_EXEC_SCRIPT nopost.sh )

    atlas_add_test( TileCondToolsTestConfig_OfflineData_test
                   SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/TileCondToolsTestConfig_OfflineData_test.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh)

    atlas_add_test( TileCondToolsTestConfig_OnlineData_test
                   SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/TileCondToolsTestConfig_OnlineData_test.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh)

    atlas_add_test( TileCondToolsTestConfig_OfflineMC_test
                   SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/TileCondToolsTestConfig_OfflineMC_test.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh)

    atlas_add_test( TileCondToolsTestConfig_OnlineMC_test
                   SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/TileCondToolsTestConfig_OnlineMC_test.py
                   PROPERTIES TIMEOUT 300
                   POST_EXEC_SCRIPT nopost.sh)

endif()
