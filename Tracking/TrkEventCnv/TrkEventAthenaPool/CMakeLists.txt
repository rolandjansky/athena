################################################################################
# Package: TrkEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrkEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Tracking/TrkEvent/TrkSpacePoint
                          GaudiKernel
                          Tracking/TrkEventCnv/TrkEventTopLevelCnv
                          Tracking/TrkEventCnv/TrkEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrkSpacePoint/SpacePointCollection.h
                           TrkSpacePoint/SpacePointContainer.h
                           TrkTrack/TrackCollection.h
                           VxMultiVertex/MVFVxContainer.h
                           VxVertex/VxContainer.h
                           TrkV0Vertex/V0Container.h
                           TrkSegment/SegmentCollection.h 
                           TYPES_WITH_NAMESPACE Trk::SegmentCollection Trk::TrackParticleBaseCollection
                           LINK_LIBRARIES
                           TrkSpacePoint
                           TrkEventTopLevelCnv
                           TrkEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )


# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRKEVENTATHENAPOOL_REFERENCE_TAG
       TrkEventAthenaPoolReference-03-00-00 )
  run_tpcnv_test( TrkEventTPCnv_14.5.0   AOD-14.5.0-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrkEventTPCnv_15.6.7   AOD-15.6.7-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrkEventTPCnv_16.6.2.1 AOD-16.6.2.1-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrkEventTPCnv_17.2.8   AOD-17.2.8-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrkEventTPCnv_18.0.0   AOD-18.0.0-full
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
  run_tpcnv_test( TrkEventTPCnv_20.1.7.2 ESD-20.1.7.2
                  REQUIRED_LIBRARIES TrackParticleAthenaPoolPoolCnv
                  REFERENCE_TAG ${TRKEVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   

