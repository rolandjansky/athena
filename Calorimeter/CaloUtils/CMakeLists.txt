# $Id: CMakeLists.txt 730049 2016-03-15 14:19:51Z krasznaa $
################################################################################
# Package: CaloUtils
################################################################################

# Declare the package name:
atlas_subdir( CaloUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloConditions
   Calorimeter/CaloDetDescr
   Calorimeter/CaloEvent
   Calorimeter/CaloGeoHelpers
   Calorimeter/CaloIdentifier
   Calorimeter/CaloInterface
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/Navigation
   Control/StoreGate
   Event/FourMom
   Event/xAOD/xAODCaloEvent
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools
   Control/CxxUtils
   Control/SGTools
   DetectorDescription/GeoModel/GeoModelInterfaces
   DetectorDescription/IdDictParser
   DetectorDescription/Identifier )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CaloUtilsLib CaloUtils/*.h CaloUtils/*.icc src/*.h src/*.cxx
   PUBLIC_HEADERS CaloUtils
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES CaloConditions CaloEvent CaloGeoHelpers CaloIdentifier
   AthenaBaseComps AthenaKernel Navigation FourMom xAODCaloEvent GaudiKernel
   CaloDetDescrLib StoreGateLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} CxxUtils
   SGTools IdDictParser Identifier )

atlas_add_component( CaloUtils src/components/*.cxx
   LINK_LIBRARIES CaloUtilsLib )

atlas_add_dictionary( CaloUtilsDict
   CaloUtils/CaloUtilsDict.h CaloUtils/selection.xml
   LINK_LIBRARIES CaloUtilsLib )

# Helper variable:
set( _jobOPath "${CMAKE_CURRENT_SOURCE_DIR}/share" )
set( _jobOPath "${_jobOPath}:${CMAKE_JOBOPT_OUTPUT_DIRECTORY}" )
set( _jobOPath "${_jobOPath}:$ENV{JOBOPTSEARCHPATH}" )

# Test(s) in the package:
atlas_add_test( CaloLayerCalculator_test
   SOURCES test/CaloLayerCalculator_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier
   StoreGateLib xAODCaloEvent GaudiKernel TestTools IdDictParser CaloUtilsLib
   EXTRA_PATTERNS "^lar decode|mask/zero|initialize_from_dict|^JobOptionsSvc +INFO|^ channel range|GP:==============|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( CaloVertexedCell_test
   SOURCES test/CaloVertexedCell_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier
   StoreGateLib GaudiKernel TestTools IdDictParser CaloUtilsLib
   EXTRA_PATTERNS "^lar decode|mask/zero|initialize_from_dict|^JobOptionsSvc +INFO|^ channel range|GP:==============|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( CaloVertexedCluster_test
   SOURCES test/CaloVertexedCluster_test.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloDetDescrLib CaloIdentifier StoreGateLib
   xAODCaloEvent GaudiKernel TestTools IdDictParser CaloUtilsLib
   EXTRA_PATTERNS "^lar decode|mask/zero|initialize_from_dict|^JobOptionsSvc +INFO|^ channel range|GP:==============|^AtlasDetectorID::"
   ENVIRONMENT "JOBOPTSEARCHPATH=${_jobOPath}" )

atlas_add_test( CaloTowerStore_test
   SCRIPT test/CaloTowerStore_test.sh
   EXTRA_PATTERNS "[0-9] [A-Z]+ 20[123][0-9]|Reading file|^CORAL/Services/ConnectionService (Info|Warning)|^RelationalDatabase Info|^Data source lookup|^DetDescrCnvSvc +INFO|^CORAL/RelationalPlugins/frontier Info|^PluginManager Info|^RalSessionMgr Warning|^DBReplicaSvc +INFO|IOVDbSvc +INFO|^Py:Athena +INFO|^EventInfoMgtInit: Got release|^TEnvRec::ChangeValue|^PoolSvc +INFO|^PoolSvc +WARNING Unable to locate catalog|being retired|including file|Deaccess DbDatabase|^IoComponentMgr +INFO|File version|Global positioning|^Domain|duplicate entry|already loaded|RootDbase: all good|frontier.c|^Py:ConfigurableDb +(WARNING|INFO)|ApplicationMgr +INFO|INFO [sS]top|^CaloIdMgrDetDes.*INFO|^TTOnlineIDDetDe.*INFO|^CaloIDHelper_ID.*INFO|^AtlasDetectorID +INFO|^AthenaEventLoopMgrWARNING|^CaloDM_IDDetDes.*INFO|^CaloLVL1_IDDetD.*INFO|^LArFCAL_ID +INFO|^LArMiniFCAL_IDD.*INFO|^LArHEC_ID +INFO|^LArMiniFCAL_ID +INFO|TileTBIdDetDesc.*INFO|^TileIDDetDescrCnv +INFO|CaloCell_IDDetD.*INFO|TileID +INFO|^LArFCAL_IDDetDe.*INFO|^TileTBIDDetDesc.*INFO|^LArEM_ID +INFO|^LArHEC_IDDetDes.*INFO|^LArEM_IDDetDesc.*INFO|^EndcapDMConst.*INFO|^GeoModelSvc +INFO|USHAPE|U-shape|XMLFileCatalog Info|No IOVSvcTool associated|^RalSessionMgr Info|RDBMS technology|INFO Found unknown streamer checksum|^GUID: Class pool::DbString has GUID|^AthenaRootStrea.* INFO|INFO TileDddbManager|^CORAL/"
   PROPERTIES TIMEOUT 500 )

atlas_add_test( CaloTowerBuilderTool_test
   SCRIPT test/CaloTowerBuilderTool_test.sh
   EXTRA_PATTERNS "[0-9] [A-Z]+ 20[123][0-9]|Reading file|^CORAL/Services/ConnectionService (Info|Warning)|^RelationalDatabase Info|^Data source lookup|^DetDescrCnvSvc +INFO|^CORAL/RelationalPlugins/frontier Info|^PluginManager Info|^RalSessionMgr Warning|^DBReplicaSvc +INFO|IOVDbSvc +INFO|^Py:Athena +INFO|^EventInfoMgtInit: Got release|^TEnvRec::ChangeValue|^PoolSvc +INFO|^PoolSvc +WARNING Unable to locate catalog|being retired|including file|Deaccess DbDatabase|^IoComponentMgr +INFO|File version|Global positioning|^Domain|duplicate entry|already loaded|RootDbase: all good|frontier.c|^Py:ConfigurableDb +(WARNING|INFO)|ApplicationMgr +INFO|INFO [sS]top|^CaloIdMgrDetDes.*INFO|^TTOnlineIDDetDe.*INFO|^CaloIDHelper_ID.*INFO|^AtlasDetectorID +INFO|^AthenaEventLoopMgrWARNING|^CaloDM_IDDetDes.*INFO|^CaloLVL1_IDDetD.*INFO|^LArFCAL_ID +INFO|^LArMiniFCAL_IDD.*INFO|^LArHEC_ID +INFO|^LArMiniFCAL_ID +INFO|TileTBIdDetDesc.*INFO|^TileIDDetDescrCnv +INFO|CaloCell_IDDetD.*INFO|TileID +INFO|^LArFCAL_IDDetDe.*INFO|^TileTBIDDetDesc.*INFO|^LArEM_ID +INFO|^LArHEC_IDDetDes.*INFO|^LArEM_IDDetDesc.*INFO|^EndcapDMConstru.*INFO|^GeoModelSvc +INFO|USHAPE|U-shape|XMLFileCatalog Info|No IOVSvcTool associated|^RalSessionMgr Info|RDBMS technology|INFO Found unknown streamer checksum|^GUID: Class pool::DbString has GUID|^AthenaRootStrea.* INFO|INFO TileDddbManager|oracle Info"
   ENVIRONMENT "ATLAS_REFERENCE_TAG=CaloUtils/CaloUtils-01-00-06"
   PROPERTIES TIMEOUT 500 )

# Install files from the package:
atlas_install_joboptions( share/*.txt share/*.py )
