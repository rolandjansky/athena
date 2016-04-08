################################################################################
# Package: LUCID_RawEvent
################################################################################

# Declare the package name:
atlas_subdir( LUCID_RawEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          PRIVATE
                          TestPolicy )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LUCID_RawEvent
                   src/*.cxx
                   PUBLIC_HEADERS LUCID_RawEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( LUCID_RawEventDict
                      LUCID_RawEvent/LUCID_RawEventDict.h
                      LUCID_RawEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools LUCID_RawEvent )

