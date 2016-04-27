################################################################################
# Package: xAODTrigMissingET
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMissingET )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTrigMissingET
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigMissingET
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODTrigMissingETDict
                      xAODTrigMissingET/xAODTrigMissingETDict.h
                      xAODTrigMissingET/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore AthLinks xAODTrigMissingET
                      EXTRA_FILES Root/dict/*.cxx )


