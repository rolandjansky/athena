################################################################################
# Package: G4AtlasTools
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Geant4 )
find_package( TBB )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4AtlasToolsLib
                   src/*.cxx
                   src/*.cc
                   PUBLIC_HEADERS G4AtlasTools
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${TBB_LIBRARIES} AthenaBaseComps G4AtlasInterfaces SubDetectorEnvelopesLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils GaudiKernel )

atlas_add_component( G4AtlasTools
                     src/components/*.cxx
                     LINK_LIBRARIES G4AtlasToolsLib )

#testing just the simulation parts, no forward detectors or region
atlas_add_test( G4GeometryToolConfig_Simtest
                SCRIPT test/G4GeometryToolConfig_Simtest.py
                PROPERTIES TIMEOUT 300 )

#full test
if( NOT SIMULATIONBASE )
  atlas_add_test( G4GeometryToolConfig_test
                SCRIPT test/G4GeometryToolConfig_test.py
                PROPERTIES TIMEOUT 300 )
endif()

atlas_add_test( G4PhysicsRegionConfig_test
                SCRIPT test/G4PhysicsRegionConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( G4FieldConfig_test
                SCRIPT test/G4FieldConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( G4AtlasToolsConfigNew_test
                SCRIPT test/G4AtlasToolsConfigNew_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

