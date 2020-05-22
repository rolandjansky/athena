# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonSimEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( MuonSimEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES MuonSimEvent/CSCSimHitCollection.h MuonSimEvent/MDTSimHitCollection.h
         MuonSimEvent/RPCSimHitCollection.h MuonSimEvent/TGCSimHitCollection.h
         MuonSimEvent/GenericMuonSimHitCollection.h
         MuonSimEvent/sTGCSimHitCollection.h MuonSimEvent/MMSimHitCollection.h
   LINK_LIBRARIES AthenaPoolUtilities AtlasSealCLHEP StoreGateLib
                  AthenaPoolCnvSvcLib GaudiKernel MuonSimEventTPCnv MuonSimEvent
                  HitManagement )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( MUONSIMEVENTATHENAPOOL_REFERENCE_TAG
       MuonSimEventAthenaPoolReference-02-01-00 )
  run_tpcnv_legacy_test( MuonSimEventTPCnv_HITS   HITS.04919495._000416
                   REFERENCE_TAG ${MUONSIMEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
