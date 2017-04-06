################################################################################
# Package: RecAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( RecAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/AtlasSealCLHEP
                          PRIVATE
                          AtlasTest/TestTools
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          GaudiKernel
                          Reconstruction/MissingETEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/RecTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( RecAthenaPoolPoolCnv
                           src/*.cxx
                           FILES MissingETEvent/MissingEtCalo.h MissingETEvent/MissingEtTruth.h MissingETEvent/MissingET.h MissingETEvent/MissingETComposition.h muonEvent/MuonSpShowerContainer.h muonEvent/MuonCaloEnergyContainer.h muonEvent/MuonContainer.h src/MuonCaloEnergies.h
                           TYPES_WITH_NAMESPACE Rec::MuonSpShowerContainer Analysis::MuonContainer TPCnv::MuonCaloEnergies
                           LINK_LIBRARIES AthenaPoolUtilities SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib GaudiKernel MissingETEvent muonEvent RecTPCnv )

# Install files from the package:
atlas_install_headers( RecAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( RECATHENAPOOL_REFERENCE_TAG
       RecAthenaPoolReference-01-00-00 )
  run_tpcnv_test( RecTPCnv_15.0.0   AOD-15.0.0-full
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( RecTPCnv_15.6.1   AOD-15.6.1-full
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( RecTPCnv_17.0.6   AOD-17.0.6-full
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( RecTPCnv_18.0.0   AOD-18.0.0-full
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
