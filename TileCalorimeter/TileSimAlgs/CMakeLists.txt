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

