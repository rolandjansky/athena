################################################################################
# Package: TileGeoG4SD
################################################################################

# Declare the package name:
atlas_subdir( TileGeoG4SD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/StoreGate
                          TileCalorimeter/TileDetDescr
                          TileCalorimeter/TileSimEvent
                          Tools/AtlasDoxygen
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/CxxUtils
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          TileCalorimeter/TileGeoModel
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( TileGeoG4SDLib
                   src/Tile*.cc
                   PUBLIC_HEADERS TileGeoG4SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} CaloIdentifier TileDetDescr TileSimEvent StoreGateLib SGtests CaloDetDescrLib G4AtlasToolsLib TileGeoModelLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CxxUtils GeoModelUtilities GaudiKernel PathResolver )

atlas_add_component( TileGeoG4SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier StoreGateLib SGtests TileDetDescr TileSimEvent CaloDetDescrLib CxxUtils GeoModelUtilities GaudiKernel G4AtlasToolsLib TileGeoModelLib PathResolver TileGeoG4SDLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/TileOpticalRatio.dat share/TileAttenuation.dat )

