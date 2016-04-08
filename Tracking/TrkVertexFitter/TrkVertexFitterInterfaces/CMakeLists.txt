################################################################################
# Package: TrkVertexFitterInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexFitterInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkVertexFitterInterfaces
                   PUBLIC_HEADERS TrkVertexFitterInterfaces
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives xAODTracking GaudiKernel TrkNeutralParameters TrkParameters )

