################################################################################
# Package: TileGeoG4SD
################################################################################

# Declare the package name:
atlas_subdir( TileGeoG4SD )

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
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} G4AtlasToolsLib TileDetDescr TileSimEvent TileG4InterfacesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier CxxUtils GeoModelInterfaces GeoModelUtilities GaudiKernel PathResolver StoreGateLib SGtests CaloDetDescrLib TileGeoModelLib )

atlas_add_component( TileGeoG4SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES TileGeoG4SDLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/TileOpticalRatio.dat share/TileAttenuation.dat )

