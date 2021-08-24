# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( RecAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES MissingETEvent/MissingEtCalo.h MissingETEvent/MissingEtTruth.h
         MissingETEvent/MissingET.h MissingETEvent/MissingETComposition.h
         muonEvent/MuonSpShowerContainer.h muonEvent/MuonCaloEnergyContainer.h
         muonEvent/MuonContainer.h src/MuonCaloEnergies.h
   TYPES_WITH_NAMESPACE Rec::MuonSpShowerContainer Analysis::MuonContainer
                        TPCnv::MuonCaloEnergies
   LINK_LIBRARIES AthenaPoolUtilities StoreGateLib AthenaPoolCnvSvcLib
                  GaudiKernel MissingETEvent muonEvent RecTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( RECATHENAPOOL_REFERENCE_TAG
       RecAthenaPoolReference-02-00-00 )
  run_tpcnv_test( RecTPCnv_15.0.0   AOD-15.0.0-full
                   REQUIRED_LIBRARIES JetEventAthenaPoolPoolCnv
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( RecTPCnv_15.6.1   AOD-15.6.1-full
                   REQUIRED_LIBRARIES JetEventAthenaPoolPoolCnv
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( RecTPCnv_17.0.6   AOD-17.0.6-full
                   REQUIRED_LIBRARIES JetEventAthenaPoolPoolCnv
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( RecTPCnv_18.0.0   AOD-18.0.0-full
                   REQUIRED_LIBRARIES JetEventAthenaPoolPoolCnv
                   REFERENCE_TAG ${RECATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
