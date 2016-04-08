################################################################################
# Package: ByteStreamData
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( ByteStreamData
                   src/ROBData.cxx
                   src/ByteStreamMetadata.cxx
                   PUBLIC_HEADERS ByteStreamData
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} DataModel SGTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( ByteStreamData_test
                   src/ROBData_t.cxx
                   PUBLIC_HEADERS ByteStreamData
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} DataModel SGTools ByteStreamData
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( ByteStreamDataDict
                      ByteStreamData/ByteStreamDataDict.h
                      ByteStreamData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} DataModel SGTools ByteStreamData ByteStreamData_test )

