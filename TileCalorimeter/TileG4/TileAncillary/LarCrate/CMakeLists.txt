################################################################################
# Package: LarCrate
################################################################################

# Declare the package name:
atlas_subdir( LarCrate )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/AtlasDoxygen
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelKernel
                          GaudiKernel
                          Simulation/G4Atlas/G4AtlasTools
                          Simulation/G4Utilities/GeoMaterial2G4 )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( LarCrate
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib SGtests GeoModelKernel GaudiKernel G4AtlasToolsLib GeoMaterial2G4 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/larcrate.dtd share/larcrate.xml share/material.dtd share/Tile_materials.xml share/LArCrate_materials.xml )

