################################################################################
# Package: xAODTrigRinger
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigRinger )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTrigCalo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTrigRinger
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigRinger
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks xAODCore xAODTrigCalo
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODTrigRingerDict
                      xAODTrigRinger/xAODTrigRingerDict.h
                      xAODTrigRinger/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODCore xAODTrigCalo xAODTrigRinger
                      EXTRA_FILES Root/dict/*.cxx )

