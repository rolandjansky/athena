# $Id: CMakeLists.txt 757711 2016-06-26 13:59:12Z ssnyder $
################################################################################
# Package: ControlTest
################################################################################

# Declare the package name:
atlas_subdir( ControlTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   AtlasTest/TestTools
   Control/AthenaExamples/AthExFortranAlgorithm
   Control/AthenaExamples/AthExStoreGateExample
   Control/AthenaPython
   Control/MinimalRunTime
   TestPolicy
   PRIVATE
   Control/CxxUtils
   Control/AthContainers
   Control/AthLinks
   Control/AthenaExamples/ToyConversion
   Control/AthenaKernel
   Control/SGTools
   Control/StoreGate
   GaudiKernel )

# Helper variable(s):
set( _jobOPath
   "${CMAKE_CURRENT_SOURCE_DIR}/share:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Test(s) in the package:
atlas_add_test( DataProxy_test
   SOURCES test/DataProxy_test.cxx
   LINK_LIBRARIES TestTools ToyConversionLib AthenaKernel SGTools GaudiKernel
   EXTRA_PATTERNS "HistogramPersis.* +INFO|EventPersistenc.*INFO|^JobOptionsSvc +INFO|IConversionSvc ptr not set"
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
   EXTRA_PATTERNS "${PPSoptignore}^ClassIDSvc +DEBUG|Histogram.* (DEBUG|INFO)|DetectorStore +DEBUG|ToyConversionSvc +DEBUG|EventDataSvc +DEBUG|EventPersis.* +(DEBUG|INFO)|HistoryStore +DEBUG|^JobOptionsSvc +INFO|^DataProxy +VERBOSE|^ProxyProviderSvc +DEBUG|^ServiceManager +DEBUG|Service base class initialized successfully|found service|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( CircularDep_test
   SOURCES test/CircularDep_test.cxx
   LINK_LIBRARIES TestTools AthContainers SGTools StoreGateLib GaudiKernel
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|Service base class initialized successfully|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( SGDataLink_test
   SOURCES test/SGDataLink_test.cxx
   LINK_LIBRARIES TestTools AthLinks AthenaKernel AthContainers SGTools StoreGateLib
   GaudiKernel
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|Service base class initialized successfully|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( ElementLink_test
   SOURCES test/ElementLink_test.cxx
   LINK_LIBRARIES TestTools AthLinks AthContainers SGTools StoreGateLib
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|Service base class initialized successfully|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( ElementLinkVector_test
   SOURCES test/ElementLinkVector_test.cxx
   LINK_LIBRARIES TestTools AthLinks AthContainers SGTools StoreGateLib GaudiKernel
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|Service base class initialized successfully|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( StoreGateSvcClient_test
   SOURCES test/StoreGateSvcClient_test.cxx
   LINK_LIBRARIES TestTools AthContainers AthenaKernel SGTools SGtests
   StoreGateLib
   EXTRA_PATTERNS "modifiable when retrieved|HistogramPersis.* +INFO|^JobOptionsSvc +INFO|Service base class initialized successfully|VERBOSE committing dataObj|DetectorStore.*error setting up proxy|DetectorStore.*record_impl: failed|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( ClearStore_test
   SOURCES test/ClearStore_test.cxx
   LINK_LIBRARIES TestTools StoreGateLib SGtests
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|Service base class initialized successfully|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( StoreID
   SOURCES test/StoreID_test.cxx
   LINK_LIBRARIES TestTools StoreGateLib
   EXTRA_PATTERNS "^JobOptionsSvc +INFO|Service base class initialized successfully|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( FortranAlgorithm
   SCRIPT test/FortranAlgorithm.sh
   EXTRA_PATTERNS "^Py:ConfigurableDb +(WARNING|INFO|ERROR)|Py:Athena +INFO including file |Warning in .TEnvRec::ChangeValue.: duplicate entry|calling initialize_|ToolSvc.finalize.. +INFO| [A-Z]+ 2[0-9][0-9][0-9]$$|^Py:Athena +INFO|[Rr]oo[Ff]it|NIKHEF|DeprecationWarning: object.__new__|^  newobj =|^\*+$$|drop-and-reload|^ *$$|we will keep the configuration around|ApplicationMgr +INFO|Start StoreGateSvc" 
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
   EXTRA_PATTERNS "^Py:ConfigurableDb +(WARNING|INFO|ERROR)|Py:Athena +INFO including file |Warning in .TEnvRec::ChangeValue.: duplicate entry|ToolSvc.finalize.. +INFO|^WriteData +INFO in initialize$$| [A-Z]+ 2[0-9][0-9][0-9]$$${SGGoptignore}|^Py:Athena +INFO|[Rr]oo[Ff]it|NIKHEF|DeprecationWarning: object.__new__|^  newobj =|^\*+$$|drop-and-reload|^ *$$|we will keep the configuration around|object not modifiable when retrieved|Retrieved const handle to default|type EventInfo|^StoreGateSvc +DEBUG|^ of type|object modifiable when retrieved|ApplicationMgr +INFO|Start StoreGateSvc" )

atlas_add_test( StoreGateGen
   SCRIPT test/StoreGateGen.sh
   PROPERTIES TIMEOUT 300
   EXTRA_PATTERNS "^Py:ConfigurableDb +(WARNING|INFO|ERROR)|Py:Athena +INFO including file |Warning in .TEnvRec::ChangeValue.: duplicate entry|ToolSvc.finalize.. +INFO|^WriteData +INFO in initialize$$| [A-Z]+ 2[0-9][0-9][0-9]$$${SGGoptignore}|^Py:Athena +INFO|[Rr]oo[Ff]it|NIKHEF|DeprecationWarning: object.__new__|^  newobj =|^\*+$$|drop-and-reload|^ *$$|we will keep the configuration around|object not modifiable when retrieved|Retrieved const handle to default|type EventInfo|^StoreGateSvc +DEBUG|^ of type|object modifiable when retrieved|ApplicationMgr +INFO|Start StoreGateSvc" )

atlas_add_test( StoreGateDFlow
   SCRIPT test/StoreGateDFlow.sh
   PROPERTIES TIMEOUT 300
   EXTRA_PATTERNS "^Py:ConfigurableDb +(WARNING|INFO|ERROR)|Py:Athena +INFO including file |Warning in .TEnvRec::ChangeValue.: duplicate entry|ToolSvc.finalize.. +INFO|^WriteData +INFO in initialize$$| [A-Z]+ 2[0-9][0-9][0-9]$$${SGGoptignore}|^Py:Athena +INFO|[Rr]oo[Ff]it|NIKHEF|DeprecationWarning: object.__new__|^  newobj =|^\*+$$|drop-and-reload|^ *$$|we will keep the configuration around|object not modifiable when retrieved|Retrieved const handle to default|type EventInfo|^StoreGateSvc +DEBUG|^ of type|object modifiable when retrieved|ApplicationMgr +INFO|WARNING could not get proxy for key|Start StoreGateSvc" )

atlas_add_test( test_interactive_athena
   SCRIPT test/test_interactive_athena.sh
   PROPERTIES TIMEOUT 300
   EXTRA_PATTERNS "^Py:ConfigurableDb +(WARNING|INFO|ERROR)|Py:Athena +INFO including file |Warning in .TEnvRec::ChangeValue.: duplicate entry|calling initialize_|ToolSvc.finalize.. +INFO| [A-Z]+ 2[0-9][0-9][0-9]$$|^Py:Athena +INFO|[Rr]oo[Ff]it|NIKHEF|DeprecationWarning: object.__new__|^  newobj =|^\*+$$|drop-and-reload|^ *$$|we will keep the configuration around|ApplicationMgr +INFO|Start StoreGateSvc|already in TClassTable" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/StoreGateTestCommon.txt share/*.py )
