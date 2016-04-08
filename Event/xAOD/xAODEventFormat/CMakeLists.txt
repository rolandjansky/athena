################################################################################
# Package: xAODEventFormat
################################################################################

# Declare the package name:
atlas_subdir( xAODEventFormat )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODCore )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODEventFormat
                   Root/*.cxx
                   PUBLIC_HEADERS xAODEventFormat
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( xAODEventFormatDict
                      xAODEventFormat/xAODEventFormatDict.h
                      xAODEventFormat/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCore xAODEventFormat )

