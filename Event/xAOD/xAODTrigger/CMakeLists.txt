################################################################################
# Package: xAODTrigger
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigger )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthContainersInterfaces
                          Control/AthLinks
                          Control/AthenaKernel
                          Event/xAOD/xAODCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTrigger
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigger
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthLinks AthenaKernel xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODTriggerDict
                      xAODTrigger/xAODTriggerDict.h
                      xAODTrigger/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks AthenaKernel xAODCore xAODTrigger
                      EXTRA_FILES Root/dict/*.cxx )

