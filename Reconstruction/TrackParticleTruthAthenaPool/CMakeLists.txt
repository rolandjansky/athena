# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackParticleTruthAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrackParticleTruthAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES ParticleTruth/TrackParticleTruthCollection.h
         ParticleTruth/TrackParticleTruthCollectionContainer.h
   LINK_LIBRARIES AthenaPoolUtilities GaudiKernel StoreGateLib
                  AthenaPoolCnvSvcLib ParticleTruth TrackParticleTruthTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
   set( TRACKPARTICLETRUTHATHENAPOOL_REFERENCE_TAG
        TrackParticleTruthAthenaPoolReference-01-00-00 )
  run_tpcnv_test( TrackParticleTruthTPCnv_18.0.0   AOD-18.0.0-full
                  REFERENCE_TAG ${TRACKPARTICLETRUTHATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
