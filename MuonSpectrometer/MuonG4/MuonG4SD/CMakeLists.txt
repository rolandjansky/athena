################################################################################
# Package: MuonG4SD
################################################################################

# Declare the package name:
atlas_subdir( MuonG4SD )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( XercesC )
find_package( GTest )

atlas_add_library( MuonG4SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS MuonG4SD
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

# Component(s) in the package:
atlas_add_component( MuonG4SD
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( CSCSensitiveDetector_gtest
                SOURCES test/CSCSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( CSCSensitiveDetectorCosmics_gtest
                SOURCES test/CSCSensitiveDetectorCosmics_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( GenericMuonSensitiveDetector_gtest
                SOURCES test/GenericMuonSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( MDTSensitiveDetector_gtest
                SOURCES test/MDTSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( MDTSensitiveDetectorCosmics_gtest
                SOURCES test/MDTSensitiveDetectorCosmics_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( MicromegasSensitiveDetector_gtest
                SOURCES test/MicromegasSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( RPCSensitiveDetector_gtest
                SOURCES test/RPCSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( RPCSensitiveDetectorCosmics_gtest
                SOURCES test/RPCSensitiveDetectorCosmics_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( TGCSensitiveDetector_gtest
                SOURCES test/TGCSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( TGCSensitiveDetectorCosmics_gtest
                SOURCES test/TGCSensitiveDetectorCosmics_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

atlas_add_test( sTGCSensitiveDetector_gtest
                SOURCES test/sTGCSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GTEST_LIBRARIES} TestTools MuonG4SDLib StoreGateLib SGtests GeoPrimitives GaudiKernel MuonSimEvent G4AtlasToolsLib MCTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/optionForTest.txt )
