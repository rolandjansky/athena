################################################################################
# Package: TileSimAlgs
################################################################################

# Declare the package name:
atlas_subdir( TileSimAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/PileUpTools
                          GaudiKernel
                          TileCalorimeter/TileIdentifier
                          TileCalorimeter/TileSimEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthAllocators
                          Control/AthenaKernel
                          Event/EventContainers
                          TileCalorimeter/TileCalib/TileCalibBlobObjs
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TilePulseSimulator
                          TileCalorimeter/TileRecUtils
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TileSimAlgs
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps PileUpToolsLib GaudiKernel TileIdentifier TileSimEvent CaloDetDescrLib CaloEvent CaloIdentifier AthAllocators AthenaKernel TileCalibBlobObjs TileConditionsLib TileDetDescr TileEvent TilePulseSimulator TileRecUtilsLib PathResolver )

# Install files from the package:
atlas_install_headers( TileSimAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

# Check python syntax:
atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python ${CMAKE_CURRENT_SOURCE_DIR}/test/*.py
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TileMuonReceiverConfig_test
                SCRIPT python -m TileSimAlgs.TileMuonReceiverConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileMuonReceiverDecisionConfig_test
                SCRIPT python -m TileSimAlgs.TileMuonReceiverDecisionConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileDigitsMakerConfig_test
                SCRIPT python -m TileSimAlgs.TileDigitsMakerConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileHitVecToCntConfig_test
                SCRIPT python -m TileSimAlgs.TileHitVecToCntConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileHitToTTL1Config_test
                SCRIPT python -m TileSimAlgs.TileHitToTTL1Config
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileDigitizationConfig_test
                SCRIPT python -m TileSimAlgs.TileDigitizationConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
