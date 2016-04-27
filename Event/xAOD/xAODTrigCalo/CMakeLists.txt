################################################################################
# Package: xAODTrigCalo
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigCalo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTrigCalo
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigCalo
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloGeoHelpers AthContainers xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODTrigCaloDict
                      xAODTrigCalo/xAODTrigCaloDict.h
                      xAODTrigCalo/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers AthContainers xAODCore AthLinks xAODTrigCalo
                      EXTRA_FILES Root/dict/*.cxx )


