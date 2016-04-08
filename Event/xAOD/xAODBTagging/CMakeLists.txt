################################################################################
# Package: xAODBTagging
################################################################################

# Declare the package name:
atlas_subdir( xAODBTagging )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/SGTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODBTagging
                   Root/*.cxx
                   PUBLIC_HEADERS xAODBTagging
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers SGTools xAODBase xAODCore xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODBTaggingDict
                      xAODBTagging/xAODBTaggingDict.h
                      xAODBTagging/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers SGTools xAODBase xAODCore xAODTracking AthLinks xAODBTagging
                      EXTRA_FILES Root/dict/*.cxx )

