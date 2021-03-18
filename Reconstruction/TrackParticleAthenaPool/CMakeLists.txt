# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackParticleAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrackParticleAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES Particle/TrackParticleContainer.h
   TYPES_WITH_NAMESPACE Rec::TrackParticleContainer
   LINK_LIBRARIES AthenaPoolUtilities StoreGateLib AthenaPoolCnvSvcLib
   GaudiKernel Particle TrackParticleTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )
if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( TRACKPARTICLEATHENAPOOL_REFERENCE_TAG
        TrackParticleAthenaPoolReference-01-00-00 )
   run_tpcnv_legacy_test( TrackParticleTPCnv_16.6.3   AOD-16.6.3-full
                   REFERENCE_TAG ${TRACKPARTICLEATHENAPOOL_REFERENCE_TAG} )
   run_tpcnv_legacy_test( TrackParticleTPCnv_17.2.8   AOD-17.2.8-full
                   REFERENCE_TAG ${TRACKPARTICLEATHENAPOOL_REFERENCE_TAG} )
   run_tpcnv_legacy_test( TrackParticleTPCnv_18.0.0   AOD-18.0.0-full
                   REFERENCE_TAG ${TRACKPARTICLEATHENAPOOL_REFERENCE_TAG} )

   set_tests_properties( TrackParticleAthenaPool_TrackParticleTPCnv_16.6.3_ctest
                         PROPERTIES TIMEOUT 900 )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
