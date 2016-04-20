################################################################################
# Package: TrkValInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkValInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkFitter/TrkFitterUtils )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkValInterfaces
                   PUBLIC_HEADERS TrkValInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel TrkEventPrimitives TrkParameters TrkTrack )

