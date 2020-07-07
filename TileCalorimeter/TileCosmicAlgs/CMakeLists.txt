################################################################################
# Package: TileCosmicAlgs
################################################################################

# Declare the package name:
atlas_subdir( TileCosmicAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Commission/CommissionEvent
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          Event/EventContainers
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Minuit2 Core Tree MathCore Hist RIO pthread MathMore Minuit Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TileCosmicAlgs
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps GaudiKernel CaloDetDescrLib CaloEvent CommissionEvent AthenaKernel Identifier TileDetDescr TileEvent TileIdentifier )

# Install files from the package:
atlas_install_headers( TileCosmicAlgs )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( TileMuonFitterConfig_test
                SCRIPT python -m TileCosmicAlgs.TileMuonFitterConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
