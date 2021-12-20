################################################################################
# Package: MuonG4SD
################################################################################

# Declare the package name:
atlas_subdir( MuonG4SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

atlas_add_library( MuonG4SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS MuonG4SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} StoreGateLib GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth MuonIdHelpersLib)

# Component(s) in the package:
atlas_add_component( MuonG4SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} MuonG4SDLib StoreGateLib GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

# Helper function setting up the tests in the package:
function( _add_test name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth
      POST_EXEC_SCRIPT nopost.sh )
endfunction()

_add_test( CSCSensitiveDetector_gtest )
_add_test( CSCSensitiveDetectorCosmics_gtest )
_add_test( GenericMuonSensitiveDetector_gtest )
_add_test( MDTSensitiveDetector_gtest )
_add_test( MDTSensitiveDetectorCosmics_gtest )
_add_test( MicromegasSensitiveDetector_gtest )
_add_test( RPCSensitiveDetector_gtest )
_add_test( RPCSensitiveDetectorCosmics_gtest )
_add_test( TGCSensitiveDetector_gtest )
_add_test( TGCSensitiveDetectorCosmics_gtest )
_add_test( sTGCSensitiveDetector_gtest )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/optionForTest.txt )
