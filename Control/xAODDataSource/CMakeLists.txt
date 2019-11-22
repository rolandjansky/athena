# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( xAODDataSource )

# Set its dependencies on other packages.
atlas_depends_on_subdirs(
   PUBLIC
   Control/xAODRootAccess )

# External dependencies.
find_package( ROOT COMPONENTS Core Tree TreePlayer RIO ROOTDataFrame )

# Build the library.
atlas_add_library( xAODDataSourceLib
   xAODDataSource/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS xAODDataSource
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess )

# Build its dictionary.
atlas_add_dictionary( xAODDataSourceDict
   xAODDataSource/xAODDataSourceDict.h xAODDataSource/selection.xml
   LINK_LIBRARIES xAODDataSourceLib )

# Build the package's test(s).
atlas_add_test( dataSourceEvent_test
   SOURCES test/dataSourceEvent_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODDataSourceLib )

atlas_add_test( dataSource_test
   SOURCES test/dataSource_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODDataSourceLib )

atlas_add_test( dataFrame_test
   SOURCES test/dataFrame_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODBase xAODDataSourceLib
   # These patterns are needed to suppress some warning messages in debug builds
   EXTRA_PATTERNS "[0-9a-f][0-9a-f] [0-9a-f][0-9a-f]|\\^~~~|vptr" )

atlas_add_test( dataFrameTypeConversion_test
   SOURCES test/dataFrameTypeConversion_test.cxx
   LINK_LIBRARIES xAODRootAccess xAODBase xAODEgamma xAODDataSourceLib
   # These patterns are needed to suppress some warning messages in debug builds
   EXTRA_PATTERNS "[0-9a-f][0-9a-f] [0-9a-f][0-9a-f]|\\^~~~|vptr" )

atlas_add_test( dataFrameElementLink_test
   SOURCES test/dataFrameElementLink_test.cxx
   LINK_LIBRARIES xAODRootAccess xAODBase xAODEgamma xAODMuon xAODDataSourceLib
   # These patterns are needed to suppress some warning messages in debug builds
   EXTRA_PATTERNS "[0-9a-f][0-9a-f] [0-9a-f][0-9a-f]|\\^~~~|vptr" )

atlas_add_test( dataFrame_pytest
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/dataFrame_test.py )

# Install files from the package.
atlas_install_python_modules( python/*.py )
