################################################################################
# Package: VP1TestSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1TestSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Coin3D )
find_package( Eigen )

include_directories( /usr/X11R6/include )

# Component(s) in the package:
atlas_add_library( VP1TestSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1TestSystems
                   INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COIN3D_LIBRARIES} VP1Base GL StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel TrkParameters TrkTrack )

