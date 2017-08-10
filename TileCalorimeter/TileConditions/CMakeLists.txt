################################################################################
# Package: TileConditions
################################################################################

# Declare the package name:
atlas_subdir( TileConditions )

atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          DetectorDescription/IdDictParser )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          TileCalorimeter/TileDetDescr
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Matrix Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Physics HistPainter Rint )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TileConditionsLib
                   src/Tile*.cxx
                   PUBLIC_HEADERS TileConditions
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel SGTools AthenaPoolUtilities Identifier GaudiKernel TileCalibBlobObjs TileIdentifier StoreGateLib SGtests CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} TileDetDescr PathResolver )

atlas_add_component( TileConditions
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel TileCalibBlobObjs TileIdentifier CaloDetDescrLib TileDetDescr PathResolver TileConditionsLib )

atlas_add_dictionary( TileConditionsDict
                      TileConditions/TileConditionsDict.h
                      TileConditions/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel TileCalibBlobObjs TileIdentifier CaloDetDescrLib TileDetDescr PathResolver TileConditionsLib )

# Test(s) in the package:
atlas_add_test( TileCondToolOfcCool_test
                SOURCES test/TileCondToolOfcCool_test.cxx
                LINK_LIBRARIES TileConditionsLib TileCalibBlobObjs AthenaBaseComps TestTools GaudiKernel ${CORAL_LIBRARIES} )

atlas_add_test( TileCondToolTMDB_test
                SOURCES test/TileCondToolTMDB_test.cxx
                LINK_LIBRARIES TileConditionsLib TileCalibBlobObjs AthenaBaseComps TestTools GaudiKernel ${CORAL_LIBRARIES} )

atlas_add_test( TileCondToolDspThreshold_test
                SOURCES test/TileCondToolDspThreshold_test.cxx
                LINK_LIBRARIES TileConditionsLib TileCalibBlobObjs AthenaBaseComps TestTools GaudiKernel ${CORAL_LIBRARIES} )

atlas_add_test( TileCablingService_test
                SOURCES test/TileCablingService_test.cxx
                LINK_LIBRARIES TileConditionsLib TileIdentifier StoreGateLib IdDictParser TestTools CxxUtils )

atlas_add_test( TileCablingService_UpgradeABC_test
                SOURCES test/TileCablingService_UpgradeABC_test.cxx
                LINK_LIBRARIES TileConditionsLib TileIdentifier StoreGateLib IdDictParser TestTools CxxUtils )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.txt share/Tile2003.* share/Tile2004.* share/TileDefault.* share/TileNo*.* share/dleak*.dat share/dsleak*.dat share/leak*.dat share/dpulse*.dat share/pulse*.dat share/noise*.dat share/ai_lo*.dat share/ai_hi*.dat share/bi_lo*.dat share/bi_hi*.dat share/ci_lo*.dat share/ci_hi*.dat share/DCS*.dat share/BadHVChannels.dat )

