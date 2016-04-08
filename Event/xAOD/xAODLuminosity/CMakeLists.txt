################################################################################
# Package: xAODLuminosity
################################################################################

# Declare the package name:
atlas_subdir( xAODLuminosity )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODLuminosity
                   Root/*.cxx
                   PUBLIC_HEADERS xAODLuminosity
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODLuminosityDict
                      xAODLuminosity/xAODLuminosityDict.h
                      xAODLuminosity/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODLuminosity
                      EXTRA_FILES Root/dict/*.cxx )

