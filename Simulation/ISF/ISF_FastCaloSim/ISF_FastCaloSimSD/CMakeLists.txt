################################################################################
# Package: ISF_FastCaloSimSD
################################################################################

# Declare the package name:
atlas_subdir( ISF_FastCaloSimSD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( ISF_FastCaloSimSD
                   src/*.cxx
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib GaudiKernel G4AtlasInterfaces G4AtlasToolsLib CaloDetDescrLib CaloIdentifier GeneratorObjects LArG4Code ISF_FastCaloSimEvent Identifier TileG4InterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

