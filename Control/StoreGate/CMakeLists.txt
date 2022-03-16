# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StoreGate )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( StoreGateLib src/*.cxx
   PUBLIC_HEADERS StoreGate
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthAllocators AthenaKernel AthContainers
   AthContainersInterfaces SGTools GaudiKernel PersistentDataModel
   PRIVATE_LINK_LIBRARIES CxxUtils TestTools )

atlas_add_component( StoreGate src/components/*.cxx
   LINK_LIBRARIES GaudiKernel StoreGateLib )

# Test library used by the package's unit tests:
atlas_add_library( SGtests test/SGtests.cxx
   NO_PUBLIC_HEADERS
   LINK_LIBRARIES CxxUtils StoreGateLib TestTools )

# Declare the package's tests:
# Helper function setting up the test(s) in the package:
function( _add_test name )
   cmake_parse_arguments( ARG "" "LOG_IGNORE_PATTERN" "" ${ARGN} )
   if( ARG_LOG_IGNORE_PATTERN )
      set( extra_arg LOG_IGNORE_PATTERN ${ARG_LOG_IGNORE_PATTERN} )
   else()
      set( extra_arg LOG_IGNORE_PATTERN "FATAL uncheckedSetTypePackageForID" ) # temporary: ATEAM-731
   endif()
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES StoreGateLib TestTools
      ${extra_arg} )
endfunction( _add_test )

set( _hivePatterns "JobOptionsSvc +INFO|StoreGateSvc.*DEBUG|HiveMgrSvc +DEBUG" )


_add_test( ActiveStore_test )
_add_test( SGTiming_test LOG_IGNORE_PATTERN "${_hivePatterns}|rec:|averages" )
_add_test( SGHive_test LOG_IGNORE_PATTERN "DataObject @0x|${_hivePatterns}" )
_add_test( DataHandle_test LOG_IGNORE_PATTERN "this proxy 0x" )
_add_test( SGIterator_test )
_add_test( KeyConcept_test )
_add_test( StoreClearedIncident_test )
_add_test( SegMemSvc_test )
_add_test( exceptions_test )
_add_test( VarHandleKey_parseKey_test )
_add_test( VarHandleKey_test )
_add_test( VarHandleKeyProperty_test )
_add_test( VarHandleProperty_test )
_add_test( ReadHandleKey_test )
_add_test( WriteHandleKey_test )
_add_test( UpdateHandleKey_test )
_add_test( TypelessWriteHandleKey_test )
_add_test( VarHandleBase_test LOG_IGNORE_PATTERN "VarHandleBase @0x" )
_add_test( VarHandles_test )
_add_test( WriteHandle_test )
_add_test( ReadHandle_test )
_add_test( UpdateHandle_test )
_add_test( TypelessWriteHandle_test )
_add_test( DecorKeyHelpers_test )
_add_test( ReadDecorHandleKey_test )
_add_test( WriteDecorHandleKey_test )
_add_test( ReadDecorHandle_test )
_add_test( WriteDecorHandle_test )
_add_test( CondHandleKeyArray_test )
_add_test( WriteCondHandle_test )
_add_test( WriteDecorHandleKeyArray_test )
_add_test( ReadDecorHandleKeyArray_test )
_add_test( ThinningHandleKey_test )
_add_test( ThinningHandleBase_test LOG_IGNORE_PATTERN "@0x" )
_add_test( ThinningHandle_test )
_add_test( ShallowCopyDecorDeps_test )


# Install files from the package:
atlas_install_joboptions( share/*.txt
   share/StoreGate_jobOptions.py )

