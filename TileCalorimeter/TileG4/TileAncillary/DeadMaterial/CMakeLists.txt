################################################################################
# Package: DeadMaterial
################################################################################

# Declare the package name:
atlas_subdir( DeadMaterial )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( DeadMaterial
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} StoreGateLib SGtests GaudiKernel G4AtlasToolsLib GeoMaterial2G4 GeoModelInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/deadmaterial.dtd share/deadmaterial.xml )

