# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkEventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TrkSpacePoint/SpacePointCollection.h
         TrkSpacePoint/SpacePointContainer.h TrkTrack/TrackCollection.h
         VxMultiVertex/MVFVxContainer.h VxVertex/VxContainer.h
         TrkV0Vertex/V0Container.h TrkSegment/SegmentCollection.h TrkEventUtils/ClusterSplitProbabilityContainer.h
   TYPES_WITH_NAMESPACE Trk::SegmentCollection Trk::TrackParticleBaseCollection Trk::ClusterSplitProbabilityContainer
   LINK_LIBRARIES TrkSpacePoint TrkEventTopLevelCnv TrkEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRKEVENTATHENAPOOL_REFERENCE_TAG
       TrkEventAthenaPoolReference-03-00-00 )
  run_tpcnv_legacy_test( TrkEventTPCnv_14.5.0   AOD-14.5.0-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( TrkEventTPCnv_15.6.7   AOD-15.6.7-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( TrkEventTPCnv_16.6.2.1 AOD-16.6.2.1-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( TrkEventTPCnv_17.2.8   AOD-17.2.8-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( TrkEventTPCnv_18.0.0   AOD-18.0.0-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_legacy_test( TrkEventTPCnv_20.1.7.2 ESD-20.1.7.2
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
