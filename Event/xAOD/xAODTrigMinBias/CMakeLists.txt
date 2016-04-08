################################################################################
# Package: xAODTrigMinBias
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMinBias )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          PRIVATE
                          Control/AthLinks
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread GenVector )

# Component(s) in the package:
atlas_add_library( xAODTrigMinBias
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigMinBias
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODTrigMinBiasDict
                      xAODTrigMinBias/xAODTrigMinBiasDict.h
                      xAODTrigMinBias/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore AthLinks xAODTrigMinBias
                      EXTRA_FILES Root/dict/*.cxx )

