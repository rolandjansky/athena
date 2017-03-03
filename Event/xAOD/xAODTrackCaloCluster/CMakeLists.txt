# $Id: CMakeLists.txt $
################################################################################
# Package: xAODTrackCaloCluster
################################################################################

# Declare the package name:
atlas_subdir( xAODTrackCaloCluster )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   DetectorDescription/GeoPrimitives
   Event/EventPrimitives
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODTracking
   Event/xAOD/xAODCaloEvent)

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( xAODTrackCaloCluster
   xAODTrackCaloCluster/*.h xAODTrackCaloCluster/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODTrackCaloCluster
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers AthLinks GeoPrimitives
   EventPrimitives xAODBase xAODCore xAODTracking xAODCaloEvent )

atlas_add_dictionary( xAODTrackCaloClusterDict
   xAODTrackCaloCluster/xAODTrackCaloClusterDict.h
   xAODTrackCaloCluster/selection.xml
   LINK_LIBRARIES xAODTrackCaloCluster 
   EXTRA_FILES Root/dict/*.cxx )

# Test(s) in the package:
#atlas_add_test( xAODTrackCaloCluster_TrackParticle_test
#   SOURCES test/xAODTrackCaloCluster_TrackParticle_test.cxx
#   LINK_LIBRARIES xAODTrackCaloCluster )
#
#atlas_add_test( xAODTrackCaloCluster_TrackParticlexAODHelpers_test
#   SOURCES test/xAODTrackCaloCluster_TrackParticlexAODHelpers_test.cxx
#   LINK_LIBRARIES xAODTrackCaloCluster
#   EXTRA_PATTERNS "^DEBUG" )
