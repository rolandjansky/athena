# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigMuonAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMuonAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTrigMuon/L2StandAloneMuonContainer.h
         xAODTrigMuon/L2StandAloneMuonAuxContainer.h
         xAODTrigMuon/L2IsoMuonContainer.h
         xAODTrigMuon/L2IsoMuonAuxContainer.h
         xAODTrigMuon/L2CombinedMuonContainer.h
         xAODTrigMuon/L2CombinedMuonAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::L2StandAloneMuonContainer
                        xAOD::L2StandAloneMuonAuxContainer
                        xAOD::L2IsoMuonContainer
                        xAOD::L2IsoMuonAuxContainer
                        xAOD::L2CombinedMuonContainer
                        xAOD::L2CombinedMuonAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
                  AthenaPoolUtilities xAODTrigMuon )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRIGMONATHENAPOOL_REFERENCE_TAG
       xAODTrigMuonAthenaPoolReference-01-01-00 )
  run_tpcnv_legacy_test( xAODTrigMuonAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODTrigMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGMONATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODTrigMuonAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrigMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRIGMONATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
