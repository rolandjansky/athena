################################################################################
# Package: RecEvent
################################################################################

# Declare the package name:
atlas_subdir( RecEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( RecEvent
                   src/*.cxx
                   PUBLIC_HEADERS RecEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( RecEventDict
                      RecEvent/RecEventDict.h
                      RecEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools RecEvent )

