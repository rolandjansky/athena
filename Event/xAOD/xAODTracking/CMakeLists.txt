################################################################################
# Package: xAODTracking
################################################################################

# Declare the package name:
atlas_subdir( xAODTracking )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTracking
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTracking
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers AthLinks GeoPrimitives EventPrimitives xAODBase xAODCore TrkNeutralParameters TrkParameters TrkTrack VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( xAODTrackingDict
                      xAODTracking/xAODTrackingDict.h
                      xAODTracking/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers AthLinks GeoPrimitives EventPrimitives xAODBase xAODCore TrkNeutralParameters TrkParameters TrkTrack VxVertex TestTools xAODTracking
                      EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( xAODTracking_TrackParticle_test
                SOURCES
                test/xAODTracking_TrackParticle_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers AthLinks GeoPrimitives EventPrimitives xAODBase xAODCore TrkNeutralParameters TrkParameters TrkTrack VxVertex TestTools xAODTracking )

atlas_add_test( xAODTracking_TrackParticlexAODHelpers_test
                SOURCES
                test/xAODTracking_TrackParticlexAODHelpers_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers AthLinks GeoPrimitives EventPrimitives xAODBase xAODCore TrkNeutralParameters TrkParameters TrkTrack VxVertex TestTools xAODTracking
                EXTRA_PATTERNS "^DEBUG" )

