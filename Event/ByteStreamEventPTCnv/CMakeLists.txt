################################################################################
# Package: ByteStreamEventPTCnv
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamEventPTCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamEventTPCnv )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( ByteStreamEventPTCnv
                         src/*.cxx
                         PUBLIC_HEADERS ByteStreamEventPTCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ByteStreamEventTPCnv )

atlas_add_dictionary( ByteStreamEventPTCnvDict
                      ByteStreamEventPTCnv/ByteStreamEventPTCnvDict.h
                      ByteStreamEventPTCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ByteStreamEventTPCnv ByteStreamEventPTCnv )

