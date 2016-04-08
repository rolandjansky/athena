################################################################################
# Package: TrkCaloCluster_OnTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkCaloCluster_OnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkCaloCluster_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkCaloCluster_OnTrack
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces TrkMaterialOnTrack )

