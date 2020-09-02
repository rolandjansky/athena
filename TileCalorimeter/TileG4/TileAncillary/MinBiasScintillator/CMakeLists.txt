################################################################################
# Package: MinBiasScintillator
################################################################################

# Declare the package name:
atlas_subdir( MinBiasScintillator )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( MinBiasScintillator
                     src/*.cc
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} CaloDetDescrLib CaloIdentifier StoreGateLib SGtests GaudiKernel G4AtlasToolsLib TileSimEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )

