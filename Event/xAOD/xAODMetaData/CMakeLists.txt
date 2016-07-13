# $Id: CMakeLists.txt 761521 2016-07-13 07:27:10Z krasznaa $
################################################################################
# Package: xAODMetaData
################################################################################

# Declare the package name:
atlas_subdir( xAODMetaData )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Event/xAOD/xAODCore )

# Component(s) in the package:
atlas_add_library( xAODMetaData
   xAODMetaData/*.h xAODMetaData/version/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS xAODMetaData
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_dictionary( xAODMetaDataDict
   xAODMetaData/xAODMetaDataDict.h
   xAODMetaData/selection.xml
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMetaData )

# Test(s) in the package:
atlas_add_test( ut_xAODFileMetaData_MetaDataType_test
   SOURCES test/ut_xAODFileMetaData_MetaDataType_test.cxx
   LINK_LIBRARIES xAODMetaData )

atlas_add_test( ut_xaodmetadata_filemetadata_eq_test
   SOURCES test/ut_xaodmetadata_filemetadata_eq_test.cxx
   LINK_LIBRARIES xAODMetaData )

# Generate CLIDs from the library:
atlas_generate_cliddb( xAODMetaData )
