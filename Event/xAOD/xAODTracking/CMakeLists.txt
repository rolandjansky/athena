# $Id: CMakeLists.txt 744435 2016-05-03 12:16:04Z krasznaa $
################################################################################
# Package: xAODTracking
################################################################################

# Declare the package name:
atlas_subdir( xAODTracking )

# Extra dependencies, based on what environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_deps Tracking/TrkEvent/TrkNeutralParameters
      Tracking/TrkEvent/TrkParameters
      Tracking/TrkEvent/TrkTrack
      Tracking/TrkEvent/VxVertex )
   set( extra_libs  TrkNeutralParameters TrkParameters TrkTrack VxVertex )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   DetectorDescription/GeoPrimitives
   Event/EventPrimitives
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   ${extra_deps} )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( xAODTracking
   xAODTracking/*.h xAODTracking/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTracking
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers AthLinks GeoPrimitives
   EventPrimitives xAODBase xAODCore ${extra_libs} )

atlas_add_dictionary( xAODTrackingDict
   xAODTracking/xAODTrackingDict.h
   xAODTracking/selection.xml
   LINK_LIBRARIES xAODTracking
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package:
atlas_add_test( xAODTracking_TrackParticle_test
   SOURCES test/xAODTracking_TrackParticle_test.cxx
   LINK_LIBRARIES xAODTracking )

atlas_add_test( xAODTracking_TrackParticlexAODHelpers_test
   SOURCES test/xAODTracking_TrackParticlexAODHelpers_test.cxx
   LINK_LIBRARIES xAODTracking
   EXTRA_PATTERNS "^DEBUG" )

atlas_add_test( ut_xaodtracking_vertex
   SOURCES test/ut_xaodtracking_vertex.cxx
   LINK_LIBRARIES xAODTracking )
