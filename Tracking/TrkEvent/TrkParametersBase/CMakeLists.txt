################################################################################
# Package: TrkParametersBase
################################################################################

# Declare the package name:
atlas_subdir( TrkParametersBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkParametersBase
                   src/*.cxx
                   PUBLIC_HEADERS TrkParametersBase
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives )

