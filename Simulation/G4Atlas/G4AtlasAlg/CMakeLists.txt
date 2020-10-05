################################################################################
# Package: G4AtlasAlg
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasAlg )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( Eigen )

# G4AtlasAlgLib library

atlas_add_library( G4AtlasAlgLib
                     src/*.cxx
                     PUBLIC_HEADERS G4AtlasAlg
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
 
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel BarcodeInterfacesLib GaudiKernel G4AtlasInterfaces SGTools StoreGateLib SGtests EventInfo GeneratorObjects GeoModelInterfaces ISF_InterfacesLib MCTruthBaseLib )

# Component(s) in the package:
atlas_add_component( G4AtlasAlg
                     src/components/*.cxx
                     PUBLIC_HEADERS G4AtlasAlg
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel GaudiKernel G4AtlasInterfaces G4AtlasAlgLib SGTools StoreGateLib SGtests EventInfo GeneratorObjects MCTruthBaseLib )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py )
