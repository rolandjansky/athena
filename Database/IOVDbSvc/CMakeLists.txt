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
find_package( nlohmann_json )
find_package( CURL )

# Component(s) in the package:
atlas_add_library( IOVDbSvcLib
   IOVDbSvc/*.h
   INTERFACE
   PUBLIC_HEADERS IOVDbSvc
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} nlohmann_json::nlohmann_json ${CURL_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} GaudiKernel nlohmann_json::nlohmann_json ${CURL_LIBRARIES}
   )

atlas_add_component( IOVDbSvc
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} nlohmann_json::nlohmann_json ${CURL_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} nlohmann_json::nlohmann_json ${CURL_LIBRARIES} GaudiKernel AthenaBaseComps AthenaKernel CxxUtils SGTools StoreGateLib
   FileCatalog AthenaPoolUtilities CoraCool IOVDbDataModel EventInfo EventInfoUtils IOVDbSvcLib 
   )


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8})
atlas_install_joboptions( share/*.py )
atlas_install_joboptions( share/*.txt )


atlas_add_test( IOVDbSvc_test
                SOURCES
                test/IOVDbSvc_test.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} ${CURL_LIBRARIES}
                PROPERTIES TIMEOUT 300
                LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG |Warning in <TFile::Init>: no |Initializing"  )
                
atlas_add_test( IOVDbSvc_Boost_test                
                SOURCES
                test/IOVDbSvc_Boost_test.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}   ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} ${CURL_LIBRARIES}
                LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG" 
                
                )
set_target_properties( IOVDbSvc_IOVDbSvc_Boost_test  PROPERTIES ENABLE_EXPORTS True )

atlas_add_test( IOVDbConn_test                
                SOURCES
                test/IOVDbConn_test.cxx src/IOVDbConn.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}   ${CURL_INCLUDE_DIRS}   
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} CoraCool ${CURL_LIBRARIES}
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( IOVDbStringFunctions_test                
                SOURCES
                test/IOVDbStringFunctions_test.cxx src/IOVDbStringFunctions.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}        ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib ${CURL_LIBRARIES} CoraCool
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( IOVDbParser_test                
                SOURCES
                test/IOVDbParser_test.cxx src/IOVDbParser.cxx  src/IOVDbStringFunctions.cxx
                INCLUDE_DIRS  ${Boost_INCLUDE_DIRS}     ${CURL_INCLUDE_DIRS}   
                LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib ${CURL_LIBRARIES}
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( FolderTypes_test                
                SOURCES
                test/FolderTypes_test.cxx src/IOVDbStringFunctions.cxx src/FolderTypes.cxx src/Json2Cool.cxx src/BasicFolder.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}  ${CURL_INCLUDE_DIRS}      
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} IOVSvcLib ${CURL_LIBRARIES} nlohmann_json::nlohmann_json
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( IOVDbCoolFunctions_test                
                SOURCES
                test/IOVDbCoolFunctions_test.cxx src/IOVDbCoolFunctions.cxx src/IOVDbConn.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}       ${CURL_INCLUDE_DIRS} 
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} IOVSvcLib  ${CURL_LIBRARIES} CoraCool
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( ReadFromFileMetaData_test                
                SOURCES
                test/ReadFromFileMetaData_test.cxx src/BasicFolder.cxx src/IOVDbSvcCurl.cxx src/Json2Cool.cxx src/CrestFunctions.cxx src/ReadFromFileMetaData.cxx src/FolderTypes.cxx src/IOVDbCoolFunctions.cxx src/IOVDbStringFunctions.cxx src/IOVDbConn.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}    ${CURL_INCLUDE_DIRS}    
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} IOVDbDataModel IOVSvcLib ${CURL_LIBRARIES} CoraCool nlohmann_json::nlohmann_json
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( IOVDbFolder_test                
                SOURCES
                test/IOVDbFolder_test.cxx src/IOVDbFolder.cxx src/CrestFunctions.cxx src/Cool2Json.cxx src/Json2Cool.cxx src/IovStore.cxx src/IOVDbConn.cxx src/IOVDbParser.cxx src/FolderTypes.cxx src/IOVDbCoolFunctions.cxx src/IOVDbStringFunctions.cxx src/ReadFromFileMetaData.cxx src/TagFunctions.cxx src/BasicFolder.cxx src/IOVDbSvcCurl.cxx src/IOVDbResolveTag.cxx src/Base64Codec.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} nlohmann_json::nlohmann_json ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} CoraCool nlohmann_json::nlohmann_json ${CURL_LIBRARIES}
                LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG"
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( IovStore_test                
                SOURCES
                test/IovStore_test.cxx  src/IovStore.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}   ${CURL_INCLUDE_DIRS}     
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} IOVSvcLib ${CURL_LIBRARIES}
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( TagFunctions_test                
                SOURCES
                test/TagFunctions_test.cxx src/TagFunctions.cxx src/IOVDbStringFunctions.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}     ${CURL_INCLUDE_DIRS}   
                LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES}  AthenaKernel  StoreGateLib  GaudiKernel TestTools EventInfo IOVSvcLib  PersistentDataModel ${COOL_LIBRARIES}  ${CURL_LIBRARIES}
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( IOVDbSvcCurl_test
                SOURCES
                test/IOVDbSvcCurl_test.cxx src/IOVDbSvcCurl.cxx 
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES}  ${CURL_LIBRARIES} IOVSvcLib
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( BasicFolder_test
                SOURCES
                test/BasicFolder_test.cxx src/BasicFolder.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}  ${COOL_INCLUDE_DIRS}  ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES}  ${COOL_LIBRARIES} IOVSvcLib ${CURL_LIBRARIES}
                POST_EXEC_SCRIPT "nopost.sh"
                )

atlas_add_test( Json2Cool_test
                SOURCES
                test/Json2Cool_test.cxx src/Json2Cool.cxx src/BasicFolder.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}  ${COOL_INCLUDE_DIRS}  ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES}  ${COOL_LIBRARIES} IOVSvcLib nlohmann_json::nlohmann_json ${CURL_LIBRARIES}
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( Cool2Json_test
                SOURCES
                test/Cool2Json_test.cxx src/Cool2Json.cxx src/Json2Cool.cxx src/IOVDbConn.cxx src/IOVDbParser.cxx src/FolderTypes.cxx src/IOVDbCoolFunctions.cxx src/IOVDbStringFunctions.cxx src/ReadFromFileMetaData.cxx src/TagFunctions.cxx src/BasicFolder.cxx src/IOVDbSvcCurl.cxx src/IOVDbResolveTag.cxx src/Base64Codec.cxx
                INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} nlohmann_json::nlohmann_json ${CURL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests GaudiKernel TestTools EventInfo IOVSvcLib xAODEventInfo PersistentDataModel ${COOL_LIBRARIES} CoraCool nlohmann_json::nlohmann_json ${CURL_LIBRARIES}
                LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG"
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( CrestFunctions_test                
                SOURCES
                test/CrestFunctions_test.cxx src/CrestFunctions.cxx src/IOVDbSvcCurl.cxx src/IOVDbStringFunctions.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}  ${CURL_INCLUDE_DIRS}  ${COOL_INCLUDE_DIRS}  
                LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib ${CURL_LIBRARIES} CoraCool
                POST_EXEC_SCRIPT "nopost.sh"
                )
                
atlas_add_test( Base64Codec_test                
                SOURCES
                test/Base64Codec_test.cxx src/Base64Codec.cxx 
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} IOVSvcLib ${CORAL_LIBRARIES}
                POST_EXEC_SCRIPT "nopost.sh"
                )


if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
  atlas_add_test( IOVDbSvcCfgTest SCRIPT python -m IOVDbSvc.IOVDbSvcConfig POST_EXEC_SCRIPT nopost.sh )
endif()


