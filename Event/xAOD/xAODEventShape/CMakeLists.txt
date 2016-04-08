################################################################################
# Package: xAODEventShape
################################################################################

# Declare the package name:
atlas_subdir( xAODEventShape )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODEventShape
                   Root/*.cxx
                   PUBLIC_HEADERS xAODEventShape
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODEventShapeDict
                      xAODEventShape/xAODEventShapeDict.h
                      xAODEventShape/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODEventShape )

