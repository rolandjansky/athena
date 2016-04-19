################################################################################
# Package: xAODCaloEvent
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Control/AthContainers
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODCaloEvent
                   Root/*.cxx
                   src/*.cxx
                   PUBLIC_HEADERS xAODCaloEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers AthContainers CxxUtils GeoPrimitives xAODBase xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODCaloEventDict
                      xAODCaloEvent/xAODCaloEventDict.h
                      xAODCaloEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent CaloGeoHelpers AthContainers CxxUtils GeoPrimitives xAODBase xAODCore AthLinks xAODCaloEvent
                      EXTRA_FILES Root/dict/*.cxx )

