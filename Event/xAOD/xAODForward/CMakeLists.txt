################################################################################
# Package: xAODForward
################################################################################

# Declare the package name:
atlas_subdir( xAODForward )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTrigL1Calo
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODForward
                   Root/*.cxx
                   PUBLIC_HEADERS xAODForward
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODCore xAODTrigL1Calo
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODForwardDict
                      xAODForward/xAODForwardDict.h
                      xAODForward/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODTrigL1Calo AthLinks xAODForward
                      EXTRA_FILES Root/dict/*.cxx )

