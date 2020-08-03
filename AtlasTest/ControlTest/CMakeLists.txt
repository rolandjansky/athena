# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ControlTest )

# Helper variable(s):
set( _jobOPath
   "${CMAKE_CURRENT_SOURCE_DIR}/share:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Test(s) in the package:
atlas_add_test( DataProxy_test
   SOURCES test/DataProxy_test.cxx
   LINK_LIBRARIES TestTools ToyConversionLib AthenaKernel SGTools GaudiKernel
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR
      "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" )
   set( PPSoptignore "requestRelease|" )
else()
   set( PPSoptignore )
endif()

atlas_add_test( ProxyProviderSvc_test
   SOURCES test/ProxyProviderSvc_test.cxx
   LINK_LIBRARIES TestTools ToyConversionLib AthenaKernel SGTools StoreGateLib
   GaudiKernel
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN "${PPSoptignore}^ClassIDSvc +DEBUG|Histogram.* (DEBUG|INFO)|DetectorStore +DEBUG|ToyConversionSvc +DEBUG|EventDataSvc +DEBUG|EventPersis.* +(DEBUG|INFO)|HistoryStore +DEBUG|^DataProxy +VERBOSE|^ProxyProviderSvc +DEBUG|^ServiceManager +DEBUG"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( CircularDep_test
   SOURCES test/CircularDep_test.cxx
   LINK_LIBRARIES TestTools AthContainers SGTools StoreGateLib GaudiKernel
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( SGDataLink_test
   SOURCES test/SGDataLink_test.cxx
   LINK_LIBRARIES TestTools AthLinks AthenaKernel AthContainers SGTools StoreGateLib
   GaudiKernel
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( ElementLink_test
   SOURCES test/ElementLink_test.cxx
   LINK_LIBRARIES TestTools AthLinks AthContainers SGTools StoreGateLib
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( ElementLinkVector_test
   SOURCES test/ElementLinkVector_test.cxx
   LINK_LIBRARIES TestTools AthLinks AthContainers SGTools StoreGateLib GaudiKernel
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( StoreGateSvcClient_test
   SOURCES test/StoreGateSvcClient_test.cxx
   LINK_LIBRARIES TestTools AthContainers AthenaKernel SGTools SGtests
   StoreGateLib
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( ClearStore_test
   SOURCES test/ClearStore_test.cxx
   LINK_LIBRARIES TestTools StoreGateLib SGtests
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( StoreID
   SOURCES test/StoreID_test.cxx
   LINK_LIBRARIES TestTools StoreGateLib
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( FortranAlgorithm
   SCRIPT test/FortranAlgorithm.sh
   PROPERTIES TIMEOUT 300 )

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR
      "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" )
   set( SGGoptignore
      "|Retrieved const handle to default object|of type EventInfo|object not modifiable when retrieved|requestRelease" )
else()
   set( SGGoptignore )
endif()

atlas_add_test( StoreGateReentrant
   SCRIPT test/StoreGateReentrant.sh
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN "Retrieved const handle to default|type EventInfo|^StoreGateSvc +DEBUG|^ of type|object modifiable when retrieved" )

atlas_add_test( StoreGateGen
   SCRIPT test/StoreGateGen.sh
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN "Retrieved const handle to default|type EventInfo|^StoreGateSvc +DEBUG|^ of type|object modifiable when retrieved" )

atlas_add_test( StoreGateDFlow
   SCRIPT test/StoreGateDFlow.sh
   PROPERTIES TIMEOUT 300 )

atlas_add_test( test_interactive_athena
   SCRIPT test/test_interactive_athena.sh
   PROPERTIES TIMEOUT 300 )

atlas_add_test( test_cfgItemList
                SCRIPT test/test_cfgItemList.sh )

atlas_add_test( test_keyStore
                SCRIPT test/test_keyStore.sh )

atlas_add_test( test_handleTest
                SCRIPT test/test_handleTest.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/StoreGateTestCommon.txt share/*.py )
