################################################################################
# Package: G4AtlasServices
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasServices )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
# Component(s) in the package:
atlas_add_component( G4AtlasServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}  ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} ${ROOT_LIBRARIES}
		     GaudiKernel AthenaBaseComps 
		     G4AtlasInterfaces G4AtlasToolsLib G4PhysicsLists PathResolver MagFieldElements MagFieldInterfaces)

#test G4AtlasFieldServices
atlas_add_test( G4AtlasFieldServices_test
                SCRIPT test/G4AtlasFieldServices_test.py
                PROPERTIES TIMEOUT 300 )

#test G4AtlasServicesConfig
atlas_add_test( G4AtlasServicesConfig_test
                SCRIPT test/G4AtlasServicesConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
