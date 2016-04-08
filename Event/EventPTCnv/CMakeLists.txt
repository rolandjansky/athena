################################################################################
# Package: EventPTCnv
################################################################################

# Declare the package name:
atlas_subdir( EventPTCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventTPCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventPTCnv
                   src/*.cxx
                   PUBLIC_HEADERS EventPTCnv
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES EventTPCnv
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( EventPTCnvDict
                      EventPTCnv/EventPTCnvDict.h
                      EventPTCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} EventTPCnv EventPTCnv )

