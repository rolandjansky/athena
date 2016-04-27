################################################################################
# Package: xAODTrigBphys
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigBphys )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODTrigBphys
                   Root/*.cxx
                   PUBLIC_HEADERS xAODTrigBphys
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODCore xAODTracking
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks )

atlas_add_dictionary( xAODTrigBphysDict
                      xAODTrigBphys/xAODTrigBphysDict.h
                      xAODTrigBphys/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODTracking AthLinks xAODTrigBphys
                      EXTRA_FILES Root/dict/*.cxx )


