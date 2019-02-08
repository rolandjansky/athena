################################################################################
# Package: LArRecUtils
################################################################################

# Declare the package name:
atlas_subdir( LArRecUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloUtils
                          Calorimeter/CaloConditions
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthAllocators
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Calorimeter/CaloGeoHelpers
                          Control/SGTools
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArCOOLConditions
			  LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRawUtils
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Eigen )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( LArRecUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArRecUtils
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} ${EIGEN_LIBRARIES} CaloEvent CaloIdentifier AthenaBaseComps AthenaKernel AthAllocators AthenaPoolUtilities Identifier GaudiKernel LArIdentifier LArRawEvent LArRecEvent CaloDetDescrLib CaloUtilsLib StoreGateLib SGtests LArCablingLib LArRawUtilsLib LArRecConditions LArRawConditions
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} TestTools CaloGeoHelpers SGTools xAODEventInfo LArCOOLConditions PathResolver CaloConditions )

atlas_add_component( LArRecUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib AthenaBaseComps AthenaKernel AthAllocators StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArCablingLib LArIdentifier LArRawConditions LArRawEvent LArRecEvent TestTools CaloGeoHelpers SGTools xAODEventInfo LArCOOLConditions LArRawUtilsLib PathResolver LArRecUtilsLib LArRecConditions LArRawConditions CaloConditions)

atlas_add_test( dummy_test
                SOURCES
                test/dummy_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib AthenaBaseComps AthenaKernel AthAllocators StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArCablingLib LArIdentifier LArRawConditions LArRawEvent LArRecEvent TestTools CaloGeoHelpers SGTools xAODEventInfo LArCOOLConditions LArRawUtilsLib CaloConditions PathResolver LArRecUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( LArFCalTowerBuilderTool
                SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/LArFCalTowerBuilderTool_test.sh
                PROPERTIES TIMEOUT 300
                EXTRA_PATTERNS "[0-9] [A-Z]+ 20[123][0-9]|Reading file|^CORAL/Services/ConnectionService (Info|Warning)|^RelationalDatabase Info|^Data source lookup|^DetDescrCnvSvc +INFO|^CORAL/RelationalPlugins/frontier Info|^PluginManager Info|^RalSessionMgr Warning|^DBReplicaSvc +INFO|IOVDbSvc +INFO|^Py:Athena +INFO|^EventInfoMgtInit: Got release|^TEnvRec::ChangeValue|^PoolSvc +INFO|being retired|including file|Deaccess DbDatabase|^IoComponentMgr +INFO|File version|Global positioning|^Domain|duplicate entry|already loaded|RootDbase: all good|frontier.c|^Py:ConfigurableDb +(WARNING|INFO)|ApplicationMgr +INFO|INFO [sS]top|^CaloIdMgrDetDes.*INFO|^TTOnlineIDDetDe.*INFO|^CaloIDHelper_ID.*INFO|^AtlasDetectorID +INFO|^AthenaEventLoopMgrWARNING|^CaloDM_IDDetDes.*INFO|^CaloLVL1_IDDetD.*INFO|^LArFCAL_ID +INFO|^LArMiniFCAL_IDD.*INFO|^LArHEC_ID +INFO|^LArMiniFCAL_ID +INFO|TileTBIdDetDesc.*INFO|^TileIDDetDescrCnv +INFO|CaloCell_IDDetD.*INFO|TileID +INFO|^LArFCAL_IDDetDe.*INFO|^TileTBIDDetDesc.*INFO|^LArEM_ID +INFO|^LArHEC_IDDetDes.*INFO|^LArEM_IDDetDesc.*INFO|^EndcapDMConstru.*INFO|^GeoModelSvc +INFO|U-shape parameter|Cs Tube parameter|XMLFileCatalog Info|No IOVSvcTool associated|^RalSessionMgr Info|RDBMS technology|^AthenaRootStre.* INFO|^GUID: Class pool::DbString has GUID|INFO TileDddbManager|^HGTD_ID *INFO|Unable to locate catalog|^CORAL/|TFile::Init|MetaDataSvc"
                 )
