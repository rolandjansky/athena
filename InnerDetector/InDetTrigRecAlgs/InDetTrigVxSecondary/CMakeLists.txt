################################################################################
# Package: InDetTrigVxSecondary
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigVxSecondary )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetTrigVxSecondary
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} xAODTracking GaudiKernel VxSecVertex TrigInterfacesLib AthContainers GeoPrimitives IRegionSelector xAODBase InDetRecToolInterfaces TrkParameters TrkTrack VxVertex TrigInDetEvent TrigSteeringEvent InDetBeamSpotServiceLib TrigInDetToolInterfacesLib )

# Install files from the package:
atlas_install_headers( InDetTrigVxSecondary )
atlas_install_python_modules( python/*.py )

