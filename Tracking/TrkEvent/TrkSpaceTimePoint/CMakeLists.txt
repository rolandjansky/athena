################################################################################
# Package: TrkSpaceTimePoint
################################################################################

# Declare the package name:
atlas_subdir( TrkSpaceTimePoint )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkSpaceTimePoint
                   src/*.cxx
                   PUBLIC_HEADERS TrkSpaceTimePoint
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkEventPrimitives
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

