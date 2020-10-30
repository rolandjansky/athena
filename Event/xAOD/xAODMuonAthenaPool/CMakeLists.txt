# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODMuonAthenaPool )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODMuonAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODMuon/MuonContainer.h xAODMuon/MuonAuxContainer.h
         xAODMuon/SlowMuonContainer.h xAODMuon/SlowMuonAuxContainer.h
         xAODMuon/MuonSegmentContainer.h xAODMuon/MuonSegmentAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::MuonContainer xAOD::MuonAuxContainer
                        xAOD::SlowMuonContainer xAOD::SlowMuonAuxContainer
                        xAOD::MuonSegmentContainer xAOD::MuonSegmentAuxContainer
   CNV_PFX xAOD
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel
                  AthenaPoolCnvSvcLib AthenaPoolUtilities xAODMuon
                  xAODTracking )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODMUONATHENAPOOL_REFERENCE_TAG
       xAODMuonAthenaPoolReference-01-01-00 )
  run_tpcnv_legacy_test( xAODMuonAthenaPool_master-20190911   AOD-master-20190911-full
                   REQUIRED_LIBRARIES xAODMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODMUONATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODMuonAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODMUONATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODMuonAthenaPool_20.7.2.2   AOD-20.7.2.2-full
                   REQUIRED_LIBRARIES xAODMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODMUONATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODMuonAthenaPool_20.7.9.9   AOD-20.7.9.9-full
                   REQUIRED_LIBRARIES xAODMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODMUONATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODMuonAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODMuonAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODMUONATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
