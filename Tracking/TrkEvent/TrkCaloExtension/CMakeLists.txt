################################################################################
# Package: TrkCaloExtension
################################################################################

# Declare the package name:
atlas_subdir( TrkCaloExtension )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODCore
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersIdentificationHelpers )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkCaloExtension
                   src/*.cxx
                   PUBLIC_HEADERS TrkCaloExtension
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers GeoPrimitives xAODCore TrkParameters TrkParametersIdentificationHelpers )

