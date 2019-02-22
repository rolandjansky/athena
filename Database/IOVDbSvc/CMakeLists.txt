# $Id: CMakeLists.txt 751303 2016-06-01 08:40:23Z krasznaa $
################################################################################
# Package: IOVDbSvc
################################################################################

# Declare the package name:
atlas_subdir( IOVDbSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/CxxUtils
   Control/SGTools
   Control/StoreGate
   Database/APR/CollectionBase
   Database/APR/CollectionUtilities
   Database/APR/POOLCore
   Database/APR/PersistencySvc
   Database/APR/StorageSvc
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/AthenaPOOL/PoolSvc
   Database/CoraCool
   Database/IOVDbDataModel
   Database/IOVDbMetaDataTools
   DetectorDescription/GeoModel/GeoModelInterfaces
   Event/EventInfo
   Event/EventInfoMgt )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core )
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( IOVDbSvcLib
   IOVDbSvc/*.h
   INTERFACE
   PUBLIC_HEADERS IOVDbSvc
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} GaudiKernel )

atlas_add_component( IOVDbSvc
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES}
   GaudiKernel AthenaBaseComps AthenaKernel CxxUtils SGTools StoreGateLib
   CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc
   AthenaPoolUtilities CoraCool IOVDbDataModel EventInfo IOVDbSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( IOVDbSvc_Boost_test                
                SOURCES
                test/IOVDbSvc_Boost_test.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} 
                EXTRA_PATTERNS "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG" 
                ENVIRONMENT  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
                )
                
set_target_properties( IOVDbSvc_IOVDbSvc_Boost_test  PROPERTIES ENABLE_EXPORTS True )

atlas_add_test( IOVDbConn_test                
                SOURCES
                test/IOVDbConn_test.cxx src/IOVDbConn.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} CoraCool
                ENVIRONMENT  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
                )

atlas_add_test( IOVDbFolder_test                
                SOURCES
                test/IOVDbFolder_test.cxx src/IOVDbFolder.cxx src/IOVDbConn.cxx src/IOVDbParser.cxx src/FolderTypes.cxx src/IOVDbCoolFunctions.cxx src/IOVDbStringFunctions.cxx src/ReadFromFileMetaData.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} CoraCool
                EXTRA_PATTERNS "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG" 
                ENVIRONMENT  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
                )
                
atlas_add_test( IOVDbParser_test                
                SOURCES
                test/IOVDbParser_test.cxx src/IOVDbParser.cxx 
                INCLUDE_DIRS  ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib 
                )

