# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

# Declare the package's tests:o
atlas_add_test( ActiveStore_test
   SOURCES test/ActiveStore_test.cxx
   LINK_LIBRARIES SGtests )

   set( _hivePatterns "JobOptionsSvc +INFO|StoreGateSvc.*DEBUG|HiveMgrSvc +DEBUG" )

atlas_add_test( SGTiming_test
   SOURCES test/SGTiming_test.cxx
   LINK_LIBRARIES SGtests
   LOG_IGNORE_PATTERN "${_hivePatterns}|rec:|averages" )

atlas_add_test( SGHive_test
   SOURCES test/SGHive_test.cxx
   LINK_LIBRARIES SGtests
   LOG_IGNORE_PATTERN "${_hivePatterns}" )

atlas_add_test( DataHandle_test
   SOURCES test/DataHandle_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( SGIterator_test
   SOURCES test/SGIterator_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( KeyConcept_test
   SOURCES test/KeyConcept_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( StoreClearedIncident_test
   SOURCES test/StoreClearedIncident_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( SegMemSvc_test
   SOURCES test/SegMemSvc_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( exceptions_test
   SOURCES test/exceptions_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleKey_parseKey_test
   SOURCES test/VarHandleKey_parseKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleKey_test
   SOURCES test/VarHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleKeyProperty_test
   SOURCES test/VarHandleKeyProperty_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleProperty_test
   SOURCES test/VarHandleProperty_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( ReadHandleKey_test
   SOURCES test/ReadHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( WriteHandleKey_test
   SOURCES test/WriteHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( UpdateHandleKey_test
   SOURCES test/UpdateHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( TypelessWriteHandleKey_test
   SOURCES test/TypelessWriteHandleKey_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandleBase_test
   SOURCES test/VarHandleBase_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( VarHandles_test
   SOURCES test/VarHandles_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( WriteHandle_test
   SOURCES test/WriteHandle_test.cxx
   LINK_LIBRARIES SGtests AthContainers )

atlas_add_test( ReadHandle_test
   SOURCES test/ReadHandle_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( UpdateHandle_test
   SOURCES test/UpdateHandle_test.cxx
   LINK_LIBRARIES SGtests )

atlas_add_test( TypelessWriteHandle_test
   SOURCES test/TypelessWriteHandle_test.cxx
   LINK_LIBRARIES SGtests AthContainers )

atlas_add_test( DecorKeyHelpers_test
   SOURCES test/DecorKeyHelpers_test.cxx
   LINK_LIBRARIES StoreGateLib )

atlas_add_test( ReadDecorHandleKey_test
   SOURCES test/ReadDecorHandleKey_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib TestTools )

atlas_add_test( WriteDecorHandleKey_test
   SOURCES test/WriteDecorHandleKey_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib TestTools )

atlas_add_test( ReadDecorHandle_test
   SOURCES test/ReadDecorHandle_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools )

atlas_add_test( WriteDecorHandle_test
   SOURCES test/WriteDecorHandle_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools )

atlas_add_test( CondHandleKeyArray_test
   SOURCES test/CondHandleKeyArray_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools )

atlas_add_test( WriteCondHandle_test
   SOURCES test/WriteCondHandle_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools )
 
atlas_add_test( WriteDecorHandleKeyArray_test
   SOURCES test/WriteDecorHandleKeyArray_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib TestTools )

atlas_add_test( ReadDecorHandleKeyArray_test
   SOURCES test/ReadDecorHandleKeyArray_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib TestTools )

atlas_add_test( ThinningHandleKey_test
   SOURCES test/ThinningHandleKey_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools )

atlas_add_test( ThinningHandleBase_test
   SOURCES test/ThinningHandleBase_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools )

atlas_add_test( ThinningHandle_test
   SOURCES test/ThinningHandle_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
   LINK_LIBRARIES StoreGateLib AthContainers TestTools )

# Install files from the package:
atlas_install_joboptions( share/*.txt
   share/StoreGate_jobOptions.py )

