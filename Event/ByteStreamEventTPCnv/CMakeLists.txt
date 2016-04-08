################################################################################
# Package: ByteStreamEventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamEventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/ByteStreamData )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( ByteStreamEventTPCnv
                         src/*.cxx
                         PUBLIC_HEADERS ByteStreamEventTPCnv
                         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ByteStreamData ByteStreamData_test )

atlas_add_dictionary( ByteStreamEventTPCnvDict
                      ByteStreamEventTPCnv/ByteStreamEventTPCnvDict.h
                      ByteStreamEventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ByteStreamData ByteStreamData_test ByteStreamEventTPCnv )

atlas_add_dictionary( OLD_ByteStreamEventTPCnvDict
                      ByteStreamEventTPCnv/ByteStreamEventTPCnvDict.h
                      ByteStreamEventTPCnv/OLD_selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ByteStreamData ByteStreamData_test ByteStreamEventTPCnv )

