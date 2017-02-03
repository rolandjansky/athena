################################################################################
# Package: xAODCaloRings
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloRings )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          Control/AthContainers
                          Control/AthLinks
                          Control/AthToolSupport/AsgTools
                          Control/SGTools
                          Event/xAOD/xAODCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODCaloRings
                   Root/*.cxx
                   PUBLIC_HEADERS xAODCaloRings
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloGeoHelpers AthContainers AthLinks AsgTools SGTools xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODCaloRingsDict
                      xAODCaloRings/xAODCaloRingsDict.h
                      xAODCaloRings/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthContainers AthLinks AsgTools SGTools xAODCore xAODCaloRings
                      EXTRA_FILES Root/dict/*.cxx )

