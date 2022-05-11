# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( APRTests )

# External dependencies:
find_package( CppUnit )
find_package( ROOT )

# Extend the include directories for all components:
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/TestDictionary )

# Component(s) in the package:
atlas_add_dictionary( test_TestDictionaryDict
   TestDictionary/dict_all.h TestDictionary/classes.xml
   NO_ROOTMAP_MERGE )

# Helper function setting up the tests of the package:
function( _add_test name )
   # Parse the optional argument(s):
   cmake_parse_arguments( ARG "" "DICTHEADER" "DEPENDS" ${ARGN} )
   # Create a dictionary if one was requested:
   if( ARG_DICTHEADER )
      atlas_add_dictionary( test_${name}Dict
         ${ARG_DICTHEADER} ${name}/classes.xml
         INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
         LINK_LIBRARIES ${Boost_LIBRARIES} PersistencySvc
         StorageSvc AthContainers
         NO_ROOTMAP_MERGE )
   endif()
   # Set up the test:
   atlas_add_test( "${name}_test"
      SOURCES ${name}/*.cpp
      INCLUDE_DIRS ${CPPUNIT_INCLUDE_DIRS}
      LINK_LIBRARIES ${CPPUNIT_LIBRARIES} PersistencySvc TestTools
      StorageSvc AthContainers  FileCatalog
      PersistentDataModel TestTools AthenaKernel
      # Need to explicitly link against ROOT libraries to suppress
      # spurious ubsan warnings.
      ${ROOT_LIBRARIES}
      ENVIRONMENT "ROOT_INCLUDE_PATH=$ENV{ROOT_INCLUDE_PATH}:${CMAKE_CURRENT_SOURCE_DIR}/TestDictionary"
      PROPERTIES TIMEOUT 300
      PRE_EXEC_SCRIPT "rm -f pool*.root *xml"
      LOG_IGNORE_PATTERN "POOL_CATALOG is not defined|using default|New file created|FID|0x[0-9a-f]{4,}"
      DEPENDS ${ARG_DEPENDS} )
endfunction( _add_test )

# Declare the test(s) of the package:
_add_test( FileCatalog_Functionality )
_add_test( StorageSvc_BasicFunctionality )
_add_test( StorageSvc_AuxStore
  DICTHEADER StorageSvc_AuxStore/AuxStoreTest.h
  DEPENDS StorageSvc_BasicFunctionality_test )
_add_test( StorageSvc_MultipleIterators
  DEPENDS StorageSvc_AuxStore_test )
_add_test( StorageSvc_ParallelReadWrite
  DEPENDS StorageSvc_MultipleIterators_test )
_add_test( StorageSvc_TransientMembers
  DICTHEADER StorageSvc_TransientMembers/TestClassWithTransients.h
  DEPENDS StorageSvc_ParallelReadWrite_test )

_add_test( PersistencySvc_Functionality
  DEPENDS StorageSvc_TransientMembers_test )
_add_test( PersistencySvc_FileCatalogOperations
  DEPENDS PersistencySvc_Functionality_test )
_add_test( PersistencySvc_FileOpenWithoutCatalog
  DEPENDS PersistencySvc_FileCatalogOperations_test )
_add_test( PersistencySvc_Parameters
  DEPENDS PersistencySvc_FileOpenWithoutCatalog_test )
_add_test( PersistencySvc_NoClassID
  DICTHEADER PersistencySvc_NoClassID/MyTestClass.h
  DEPENDS PersistencySvc_Parameters_test )

# Clean up:
unset( _add_test )
