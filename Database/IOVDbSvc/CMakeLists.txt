# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbSvc )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase RelationalAccess )
find_package( ROOT COMPONENTS Core )
find_package( Boost COMPONENTS unit_test_framework regex )
find_package( nlohmann_json )
find_package( CURL )

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
   ${Boost_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES}
   ${Boost_LIBRARIES} ${CURL_LIBRARIES} nlohmann_json::nlohmann_json GaudiKernel
   AthenaBaseComps AthenaKernel StoreGateLib FileCatalog AthenaPoolUtilities
   CoraCool IOVDbDataModel EventInfo EventInfoUtils GeoModelInterfaces
   IOVDbMetaDataToolsLib EventInfoMgtLib PoolSvcLib IOVDbSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8})
atlas_install_joboptions( share/*.py )
atlas_install_joboptions( share/*.txt )

# Test(s) in the package.
atlas_add_test( IOVDbSvc_test
   SOURCES test/IOVDbSvc_test.cxx
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES}
   GaudiKernel AthenaKernel CxxUtils TestTools
   PROPERTIES TIMEOUT 300
   LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG |Warning in <TFile::Init>: no |Initializing"  )

atlas_add_test( IOVDbSvc_Boost_test
   SOURCES test/IOVDbSvc_Boost_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${ROOT_LIBRARIES}
   GaudiKernel AthenaKernel CxxUtils TestTools StoreGateLib
   LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG" )

atlas_add_test( IOVDbConn_test
   SOURCES test/IOVDbConn_test.cxx src/IOVDbConn.cxx
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} ${Boost_LIBRARIES} ${ROOT_LIBRARIES}
   GaudiKernel CxxUtils TestTools CoraCool
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( IOVDbStringFunctions_test
   SOURCES test/IOVDbStringFunctions_test.cxx src/IOVDbStringFunctions.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES}
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( IOVDbParser_test
   SOURCES test/IOVDbParser_test.cxx src/IOVDbParser.cxx
   src/IOVDbStringFunctions.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel CxxUtils
   TestTools
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( FolderTypes_test
   SOURCES test/FolderTypes_test.cxx src/FolderTypes.cxx
   src/IOVDbStringFunctions.cxx
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${Boost_LIBRARIES}
   ${ROOT_LIBRARIES} GaudiKernel AthenaKernel StoreGateLib CxxUtils TestTools
   AthenaPoolUtilities
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( IOVDbCoolFunctions_test
   SOURCES test/IOVDbCoolFunctions_test.cxx src/IOVDbCoolFunctions.cxx
   src/IOVDbConn.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   GaudiKernel AthenaKernel CoraCool
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( ReadFromFileMetaData_test
   SOURCES test/ReadFromFileMetaData_test.cxx src/ReadFromFileMetaData.cxx
   src/FolderTypes.cxx src/IOVDbCoolFunctions.cxx src/IOVDbStringFunctions.cxx
   src/IOVDbConn.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   AthenaKernel IOVDbDataModel AthenaPoolUtilities CoraCool CxxUtils TestTools
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( IOVDbFolder_test
   SOURCES test/IOVDbFolder_test.cxx src/FolderTypes.cxx
   src/IOVDbStringFunctions.cxx src/IOVDbParser.cxx src/IOVDbConn.cxx
   src/IOVDbFolder.cxx src/IovStore.cxx
   src/ReadFromFileMetaData.cxx src/IOVDbCoolFunctions.cxx src/TagFunctions.cxx
   src/Cool2Json.cxx src/Base64Codec.cxx src/Json2Cool.cxx src/IOVDbSvcCurl.cxx
   src/BasicFolder.cxx src/IOVDbResolveTag.cxx src/CrestFunctions.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   ${ROOT_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   ${ROOT_LIBRARIES} ${CURL_LIBRARIES} nlohmann_json::nlohmann_json
   GaudiKernel AthenaKernel AthenaBaseComps StoreGateLib SGTools CxxUtils
   TestTools CoraCool AthenaPoolUtilities GeoModelInterfaces EventInfo
   IOVDbMetaDataToolsLib IOVDbDataModel
   LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG"
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( IovStore_test
   SOURCES test/IovStore_test.cxx src/IovStore.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( TagFunctions_test
   SOURCES test/TagFunctions_test.cxx src/TagFunctions.cxx
   src/IOVDbStringFunctions.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${ROOT_LIBRARIES}
   GaudiKernel AthenaKernel StoreGateLib AthenaPoolUtilities CxxUtils TestTools
   EventInfo GeoModelInterfaces
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( IOVDbSvcCurl_test
   SOURCES test/IOVDbSvcCurl_test.cxx src/IOVDbSvcCurl.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${CURL_LIBRARIES}
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( BasicFolder_test
   SOURCES test/BasicFolder_test.cxx src/BasicFolder.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( Json2Cool_test
   SOURCES test/Json2Cool_test.cxx src/Json2Cool.cxx src/BasicFolder.cxx
   src/IOVDbStringFunctions.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   nlohmann_json::nlohmann_json
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( Cool2Json_test
   SOURCES test/Cool2Json_test.cxx src/Cool2Json.cxx src/IOVDbConn.cxx
   src/IOVDbParser.cxx src/IOVDbStringFunctions.cxx src/FolderTypes.cxx
   src/Base64Codec.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   ${ROOT_LIBRARIES} GaudiKernel AthenaKernel StoreGateLib CoraCool
   AthenaPoolUtilities CxxUtils TestTools
   LOG_IGNORE_PATTERN "^HistogramPersis.* INFO|^IOVSvc +DEBUG|^IOVSvcTool +DEBUG"
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( CrestFunctions_test
   SOURCES test/CrestFunctions_test.cxx src/CrestFunctions.cxx
   src/IOVDbSvcCurl.cxx src/IOVDbStringFunctions.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   ${CURL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES}
   ${CURL_LIBRARIES}
   POST_EXEC_SCRIPT "nopost.sh" )

atlas_add_test( Base64Codec_test
   SOURCES test/Base64Codec_test.cxx src/Base64Codec.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES}
   POST_EXEC_SCRIPT "nopost.sh" )

if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
   atlas_add_test( IOVDbSvcCfgTest
      SCRIPT python -m IOVDbSvc.IOVDbSvcConfig
      POST_EXEC_SCRIPT "nopost.sh" )
endif()
