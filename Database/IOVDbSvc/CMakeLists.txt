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
   Database/APR/FileCatalog
   Database/AthenaPOOL/AthenaPoolUtilities
   Database/AthenaPOOL/PoolSvc
   Database/CoraCool
   Database/IOVDbDataModel
   Database/IOVDbMetaDataTools
   DetectorDescription/GeoModel/GeoModelInterfaces
   Event/EventInfo
   Event/EventInfoUtils
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
   FileCatalog AthenaPoolUtilities CoraCool IOVDbDataModel EventInfo EventInfoUtils IOVDbSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


atlas_add_test( IOVDbSvc_test
                SOURCES
                test/IOVDbSvc_test.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
                LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES}
                PROPERTIES TIMEOUT 300
                EXTRA_PATTERNS "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG |Warning in <TFile::Init>: no" 
                ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
                
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
                
atlas_add_test( IOVDbStringFunctions_test                
                SOURCES
                test/IOVDbStringFunctions_test.cxx src/IOVDbStringFunctions.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib
                )
                
atlas_add_test( IOVDbParser_test                
                SOURCES
                test/IOVDbParser_test.cxx src/IOVDbParser.cxx 
                INCLUDE_DIRS  ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib 
                )
                
atlas_add_test( FolderTypes_test                
                SOURCES
                test/FolderTypes_test.cxx src/IOVDbStringFunctions.cxx src/FolderTypes.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} IOVSvcLib 
                )
                
atlas_add_test( IOVDbCoolFunctions_test                
                SOURCES
                test/IOVDbCoolFunctions_test.cxx src/IOVDbCoolFunctions.cxx 
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} IOVSvcLib 
                )
                
atlas_add_test( ReadFromFileMetaData_test                
                SOURCES
                test/ReadFromFileMetaData_test.cxx src/ReadFromFileMetaData.cxx src/FolderTypes.cxx src/IOVDbCoolFunctions.cxx src/IOVDbStringFunctions.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} IOVDbDataModel IOVSvcLib 
                )
                
atlas_add_test( IOVDbFolder_test                
                SOURCES
                test/IOVDbFolder_test.cxx src/IOVDbFolder.cxx src/IOVDbConn.cxx src/IOVDbParser.cxx src/FolderTypes.cxx src/IOVDbCoolFunctions.cxx src/IOVDbStringFunctions.cxx src/ReadFromFileMetaData.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}        
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} CoraCool
                EXTRA_PATTERNS "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG" 
                ENVIRONMENT  "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share"
                )


if( NOT SIMULATIONBASE )
  atlas_add_test( IOVDbSvcCfgTest SCRIPT python -m IOVDbSvc.IOVDbSvcConfig POST_EXEC_SCRIPT nopost.sh )
endif()
