################################################################################
# Package: TileSimAlgs
################################################################################

# Declare the package name:
atlas_subdir( TileSimAlgs )

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
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
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
