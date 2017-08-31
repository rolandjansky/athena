################################################################################
# Package: TileRawUtils
################################################################################

# Declare the package name:
atlas_subdir( TileRawUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          DetectorDescription/Identifier
                          GaudiKernel
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/StoreGate
                          DetectorDescription/RegionSelector
                          TileCalorimeter/TileConditions
                          TileCalorimeter/TileDetDescr
			  TileCalorimeter/TileSvc/TileByteStream )

# Component(s) in the package:
atlas_add_component( TileRawUtils
                     src/Tile*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloIdentifier AthenaBaseComps Identifier GaudiKernel TileEvent TileIdentifier CaloDetDescrLib StoreGateLib SGtests RegionSelectorLib TileConditionsLib TileDetDescr TileByteStreamLib )

# Install files from the package:
atlas_install_headers( TileRawUtils )

