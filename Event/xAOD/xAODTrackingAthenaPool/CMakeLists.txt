# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrackingAthenaPool )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrackingAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODTracking/TrackParticleContainer.h
   xAODTracking/TrackParticleAuxContainer.h
   xAODTracking/NeutralParticleContainer.h
   xAODTracking/NeutralParticleAuxContainer.h xAODTracking/VertexContainer.h
   xAODTracking/VertexAuxContainer.h xAODTracking/SCTRawHitValidationContainer.h
   xAODTracking/SCTRawHitValidationAuxContainer.h
   xAODTracking/TrackMeasurementValidationContainer.h
   xAODTracking/TrackMeasurementValidationAuxContainer.h
   xAODTracking/TrackStateValidationContainer.h
   xAODTracking/TrackStateValidationAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::TrackParticleContainer
   xAOD::TrackParticleAuxContainer xAOD::NeutralParticleContainer
   xAOD::NeutralParticleAuxContainer xAOD::VertexContainer
   xAOD::VertexAuxContainer xAOD::SCTRawHitValidationContainer
   xAOD::SCTRawHitValidationAuxContainer
   xAOD::TrackMeasurementValidationContainer
   xAOD::TrackMeasurementValidationAuxContainer
   xAOD::TrackStateValidationContainer xAOD::TrackStateValidationAuxContainer
   CNV_PFX xAOD
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthenaKernel
   AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTracking xAODTrackingCnvLib
   xAODTruth )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( XAODTRACKINGATHENAPOOL_REFERENCE_TAG
       xAODTrackingAthenaPoolReference-02-00-00 )
  run_tpcnv_legacy_test( xAODTrackingAthenaPool_20.1.7.2   AOD-20.1.7.2-full
                   REQUIRED_LIBRARIES xAODTrackingAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRACKINGATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODTrackingAthenaPool_20.7.2.2   AOD-20.7.2.2-full
                   REQUIRED_LIBRARIES xAODTrackingAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRACKINGATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODTrackingAthenaPool_21.0.79   AOD-21.0.79-full
                   REQUIRED_LIBRARIES xAODTrackingAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRACKINGATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( xAODTrackingAthenaPool_master-20190911   AOD-master-20190911-full
                   REQUIRED_LIBRARIES xAODTrackingAthenaPoolPoolCnv
                   REFERENCE_TAG ${XAODTRACKINGATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
