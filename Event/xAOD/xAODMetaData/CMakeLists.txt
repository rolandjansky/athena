# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODMetaData )

# Pull in the helper CMake code.
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODMetaData
   xAODMetaData/*.h xAODMetaData/version/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS xAODMetaData
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODMetaData/selection.xml
   OUTPUT _selectionFile
   OBJECTS "xAOD::FileMetaData_v1" )

atlas_add_dictionary( xAODMetaDataDict
   xAODMetaData/xAODMetaDataDict.h
   ${_selectionFile}
   LINK_LIBRARIES xAODCore xAODMetaData )

# Test(s) in the package.
atlas_add_test( ut_xAODFileMetaData_MetaDataType_test
   SOURCES test/ut_xAODFileMetaData_MetaDataType_test.cxx
   LINK_LIBRARIES xAODMetaData )

atlas_add_test( ut_xaodmetadata_filemetadata_eq_test
   SOURCES test/ut_xaodmetadata_filemetadata_eq_test.cxx
   LINK_LIBRARIES xAODMetaData )

# Generate CLIDs from the library.
atlas_generate_cliddb( xAODMetaData )
