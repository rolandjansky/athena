################################################################################
# Package: TrkEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrkEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkV0Vertex
                          Tracking/TrkEvent/VxMultiVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkEventCnv/TrkEventCnvTools
                          Tracking/TrkEventCnv/TrkEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrkSpacePoint/SpacePointCollection.h TrkSpacePoint/SpacePointContainer.h TrkTrack/TrackCollection.h VxMultiVertex/MVFVxContainer.h VxVertex/VxContainer.h TrkV0Vertex/V0Container.h TrkSegment/SegmentCollection.h
                           TYPES_WITH_NAMESPACE Trk::SegmentCollection Trk::TrackParticleBaseCollection
                           LINK_LIBRARIES AthenaPoolUtilities StoreGateLib SGtests AthenaPoolCnvSvcLib AtlasDetDescr GaudiKernel TrkDetElementBase TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkSpacePoint TrkTrack TrkV0Vertex VxMultiVertex VxVertex TrkEventTPCnv )

# Install files from the package:
atlas_install_headers( TrkEventAthenaPool )
atlas_install_joboptions( share/*.py )


# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRKEVENTATHENAPOOL_REFERENCE_TAG
       TrkEventAthenaPoolReference-01-01-00 )
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
