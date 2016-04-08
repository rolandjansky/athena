################################################################################
# Package: TrkVertexOnTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexOnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkVertexOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexOnTrack
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives GaudiKernel TrkSurfaces TrkMeasurementBase TrkParameters
                   PRIVATE_LINK_LIBRARIES VxVertex )

