################################################################################
# Package: DataModelRoot
################################################################################

# Declare the package name:
atlas_subdir( DataModelRoot )

atlas_depends_on_subdirs (
  PUBLIC
  Control/CxxUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DataModelRoot
                   src/*.cxx
                   PUBLIC_HEADERS DataModelRoot
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( DataModelRootDict
                      DataModelRoot/DataModelRootDict.h
                      DataModelRoot/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModelRoot )

