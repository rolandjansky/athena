# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaServices )

# External dependencies:
find_package( Boost COMPONENTS thread )
find_package( CLHEP )
find_package( Python COMPONENTS Development )
find_package( yampl )

# Component(s) in the package:
atlas_add_component( AthenaServices src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${Python_INCLUDE_DIRS}
   ${CLHEP_INCLUDE_DIRS} ${YAMPL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${Python_LIBRARIES} ${YAMPL_LIBRARIES}
   ${CLHEP_LIBRARIES} z TestTools AsgTools AthenaBaseComps AthenaKernel RootUtils CxxUtils
   AthContainers AthContainersInterfaces DataModelRoot Navigation PerfMonEvent PerfMonKernel SGTools
   StoreGateLib SGtests PersistentDataModel EventInfo xAODCore xAODEventInfo EventInfoUtils GaudiKernel )

# Test library checking the ability to build T/P converters:
atlas_add_tpcnv_library( AthenaServicesTest src/test/*.cxx
   NO_PUBLIC_HEADERS
   PRIVATE_LINK_LIBRARIES AthenaKernel)

# The test(s) of the package:
atlas_add_test( AthenaOutputStream_test
   SOURCES test/AthenaOutputStream_test.cxx src/AthenaOutputStream.cxx
   src/OutputStreamSequencerSvc.cxx src/MetaDataSvc.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES TestTools AsgTools AthenaKernel SGTools StoreGateLib GaudiKernel AthenaBaseComps PersistentDataModel
   LOG_IGNORE_PATTERN "^AthenaRootStrea... +(INFO|DEBUG)|^SGAudSvc +(INFO|DEBUG)|of type DataHistory|DEBUG Recorded object|object modifiable when retrieved|^ToolSvc +DEBUG Service base class initialized|^ServiceManager +DEBUG Initializing service|^IncidentSvc *DEBUG Adding .* listener|DecisionSvc +DEBUG|^IoComponentMgr +(INFO|DEBUG)|DBReplicaSvc|^HistogramPersis.*DEBUG|^ItemListSvc +(INFO|DEBUG)|Info File PoolFileCatalog.xml does not exist|DataModelCompatSvc::initialize|^ProxyProviderSvc +DEBUG|^DataModelCompatSvc +DEBUG|^AthenaOutputStreamVERBOSE|^AthenaOutputStream +DEBUG|^TimelineSvc +DEBUG|CLIDRegistry entries" )
# Avoid spurious ubsan warnings.
set_target_properties( AthenaServices_AthenaOutputStream_test PROPERTIES ENABLE_EXPORTS True )

atlas_add_test( FPEControlSvc_test
   SOURCES test/FPEControlSvc_test.cxx
   LINK_LIBRARIES TestTools AthenaKernel GaudiKernel )

atlas_add_test( AthenaEventLoopMgr_test
   SOURCES test/AthenaEventLoopMgr_test.cxx
   LINK_LIBRARIES TestTools AsgTools AthenaKernel GaudiKernel EventInfo AthenaBaseComps )

atlas_add_test( ConditionsCleanerSvc_test
  SOURCES test/ConditionsCleanerSvc_test.cxx
  LINK_LIBRARIES TestTools AthenaKernel GaudiKernel AthenaBaseComps )

atlas_add_test( RCUSvc_test
   SOURCES test/RCUSvc_test.cxx
   LINK_LIBRARIES TestTools AsgTools AthenaKernel GaudiKernel EventInfo AthenaBaseComps )

atlas_add_test( DelayedConditionsCleanerSvc_test
   SOURCES test/DelayedConditionsCleanerSvc_test.cxx
   LINK_LIBRARIES TestTools AsgTools AthenaKernel GaudiKernel EventInfo AthenaBaseComps )

atlas_add_test( ThinningCacheTool_test
   SOURCES test/ThinningCacheTool_test.cxx
   LINK_LIBRARIES TestTools AsgTools AthenaKernel GaudiKernel EventInfo AthenaBaseComps )

atlas_add_test( TestStopRun
   SCRIPT test/TestStopRun.sh
   PROPERTIES TIMEOUT 300 )

atlas_add_test( AthTPCnvSvc
   SCRIPT test/AthTPCnvSvc.sh
   LOG_IGNORE_PATTERN "^ApplicationMgr +INFO"
   PROPERTIES TIMEOUT 300 )

atlas_add_test( SimplePOSIXTimeKeeperSvc
   SCRIPT test/SimplePOSIXTimeKeeperSvc.sh
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN "allocated job time|average time|start processing|done processing|waste some time|all times in|start of run|BeginRun" )

atlas_add_test( AthDictLoaderSvc
   SCRIPT test/AthDictLoaderSvc.sh
   PROPERTIES TIMEOUT 300 )


# Install files from the package:
atlas_install_python_modules( python/*.py
   POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_install_joboptions( share/AthTPCnvSvc_test.py
   share/SimplePOSIXTimeKeeperOptions.py
   share/SimplePOSIXTimeKeeperSvc_test.py
   share/MixingEventSelector_test.py
   share/MultiplePassBootstrap.py
   share/MultiplePass_test.py
   share/MultiplePassWithAlgFilter_test.py
   share/OverrideEventNumber.py
   share/TestStopRun.py
   share/TestSeedRunEvent.py
   share/AthDictLoaderSvc_test.py
   share/ReadAthenaPoolSeek_jobOptions.py
   share/AthenaJobOptionsSvc_jobOptions.py
   share/test_tpcnvdb.py 
   share/AthenaOutputStream_test.txt 
   share/FPEControlSvc_test.txt 
   share/AthenaEventLoopMgr_test.txt 
   share/ConditionsCleanerSvc_test.txt
   share/RCUSvc_test.txt
   share/DelayedConditionsCleanerSvc_test.txt
   share/ThinningCacheTool_test.txt )

# Need to make sure that python file installation happens before genconf
# in order for our custom __init__.py to be installed correctly.
add_dependencies( AthenaServicesConfigurables AthenaServicesPythonInstall )
