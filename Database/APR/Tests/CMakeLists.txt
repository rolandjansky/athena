# $Id: CMakeLists.txt 787458 2016-11-30 21:15:29Z ssnyder $
################################################################################
# Package: Tests
################################################################################

# Declare the package name:
atlas_subdir( Tests )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Database/APR/FileCatalog
   Database/APR/PersistencySvc
   Database/APR/StorageSvc
   Database/PersistentDataModel
   Control/AthContainers
   Control/AthContainersInterfaces
   AtlasTest/TestTools )

# External dependencies:
find_package( CppUnit )

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
         LINK_LIBRARIES ${Boost_LIBRARIES} FileCatalog PersistencySvc
         StorageSvc AthContainers
         NO_ROOTMAP_MERGE )
   endif()
   # Set up the test properties based on the optional argument(s):
   set( properties )
   if( ARG_DEPENDS )
     list( APPEND properties PROPERTIES DEPENDS "${ARG_DEPENDS}" )
   endif()
   # Set up the test:
   atlas_add_test( "${name}_test"
      SOURCES ${name}/*.cpp
      INCLUDE_DIRS ${CPPUNIT_INCLUDE_DIRS}
      LINK_LIBRARIES ${CPPUNIT_LIBRARIES} FileCatalog PersistencySvc
      StorageSvc AthContainers
      PersistentDataModel TestTools
      PRE_EXEC_SCRIPT "rm -f pool*.root *xml"
      POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/post.sh ${name}_test"
      ${properties} )
endfunction( _add_test )

# Declare the test(s) of the package:
_add_test( FileCatalog_Functionality )
_add_test( StorageSvc_BasicFunctionality )
_add_test( StorageSvc_AuxStore
  DICTHEADER StorageSvc_AuxStore/AuxStoreTest.h
  DEPENDS Tests_StorageSvc_BasicFunctionality_test_ctest )
_add_test( StorageSvc_MultipleIterators
  DEPENDS Tests_StorageSvc_AuxStore_test_ctest )
_add_test( StorageSvc_ParallelReadWrite
  DEPENDS Tests_StorageSvc_MultipleIterators_test_ctest )
_add_test( StorageSvc_TransientMembers
  DICTHEADER StorageSvc_TransientMembers/TestClassWithTransients.h
  DEPENDS Tests_StorageSvc_ParallelReadWrite_test_ctest )

_add_test( PersistencySvc_Functionality
  DEPENDS Tests_StorageSvc_TransientMembers_test_ctest )
_add_test( PersistencySvc_FileCatalogOperations
  DEPENDS Tests_PersistencySvc_Functionality_test_ctest )
_add_test( PersistencySvc_FileOpenWithoutCatalog
  DEPENDS Tests_PersistencySvc_FileCatalogOperations_test_ctest )
_add_test( PersistencySvc_Parameters
  DEPENDS Tests_PersistencySvc_FileOpenWithoutCatalog_test_ctest )
_add_test( PersistencySvc_NoClassID
  DICTHEADER PersistencySvc_NoClassID/MyTestClass.h
  DEPENDS Tests_PersistencySvc_Parameters_test_ctest )

# Clean up:
unset( _add_test )
