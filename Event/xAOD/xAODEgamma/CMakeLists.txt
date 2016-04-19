################################################################################
# Package: xAODEgamma
################################################################################

# Declare the package name:
atlas_subdir( xAODEgamma )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/CxxUtils
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODPrimitives
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

athena_install_nonstandard_headers(xAODEgamma/versions)

# Component(s) in the package:
atlas_add_library( xAODEgamma
                   Root/*.cxx
                   PUBLIC_HEADERS xAODEgamma
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers AthLinks CxxUtils SGTools GeoPrimitives EventPrimitives xAODBase xAODCaloEvent xAODCore xAODPrimitives xAODTracking xAODTruth
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODEgammaDict
                      xAODEgamma/xAODEgammaDict.h
                      xAODEgamma/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthContainers AthLinks CxxUtils SGTools GeoPrimitives EventPrimitives xAODBase xAODCaloEvent xAODCore xAODPrimitives xAODTracking xAODTruth xAODEgamma
                      EXTRA_FILES Root/dict/*.cxx )

