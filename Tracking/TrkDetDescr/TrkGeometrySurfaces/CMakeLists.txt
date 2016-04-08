################################################################################
# Package: TrkGeometrySurfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkGeometrySurfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParametersBase
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkGeometrySurfaces
                   src/*.cxx
                   PUBLIC_HEADERS TrkGeometrySurfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkDetDescrUtils TrkSurfaces TrkEventPrimitives TrkParametersBase
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

