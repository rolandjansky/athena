################################################################################
# Package: xAODMetaData
################################################################################

# Declare the package name:
atlas_subdir( xAODMetaData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Event/xAOD/xAODCore
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( xAODMetaData
                   Root/*.cxx
                   PUBLIC_HEADERS xAODMetaData
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers xAODCore
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( xAODMetaDataDict
                      xAODMetaData/xAODMetaDataDict.h
                      xAODMetaData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore TestTools xAODMetaData )

atlas_add_test( ut_xAODFileMetaData_MetaDataType_test
                SOURCES
                test/ut_xAODFileMetaData_MetaDataType_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore TestTools xAODMetaData )

