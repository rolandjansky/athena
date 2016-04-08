################################################################################
# Package: TrigInDetPattRecoEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigInDetPattRecoEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrigInDetPattRecoEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetPattRecoEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives TrkParameters TrigInDetEvent TrigSteeringEvent )

