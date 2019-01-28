/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "MuonGeoModel/MuonDetectorFactory001.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/FPVMAP.h"
#include "MuonGeoModel/MuonSystemDescription.h"
#include "MuonGeoModel/StationSelector.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MuonChamber.h"
#include "MuonReadoutGeometry/GenericCSCCache.h"
#include "MuonReadoutGeometry/GenericMDTCache.h"
#include "MuonReadoutGeometry/GenericRPCCache.h"
#include "MuonReadoutGeometry/GenericTGCCache.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "IdDictDetDescr/IdDictManager.h" 
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
// for nSW
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoSerialTransformer.h" 
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPerfUtils.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoGenericFunctions/Variable.h"

#include "MuonGeoModel/DBReader.h"
#include "MuonGeoModel/RDBReaderAtlas.h"

#include <stdexcept>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
typedef std::stringstream  my_sstream;
typedef std::ostringstream my_osstream;

// The objects for mapping plane indexes in MuonSystem to the record index in RDBRecordset
typedef std::map<int, unsigned int, std::less<int> > muonsysIndMap;

using namespace GeoGenfun;
using namespace GeoXF;

#define skip_chambers  false
#define skip_toroids   false
#define skip_shielding false
#define skip_caloSaddle true
#define useAssemblies  false


namespace MuonGM {

  MuonDetectorFactory001::MuonDetectorFactory001(StoreGateSvc* pDetStore) 
    : m_includeCutouts(0), m_includeCutoutsBog(0), m_includeCtbBis(0), m_rdb(1), m_controlAlines(0),
      m_minimalGeoFlag(0), m_controlCscIntAlines(0), m_dumpAlines(false), m_dumpCscIntAlines(false),
      m_useCscIntAlinesFromGM(true), m_caching(0), m_cacheFillingFlag(0), m_mdtDeformationFlag(0),
      m_mdtAsBuiltParaFlag(0), m_dumpMemoryBreakDown(false), m_muon(NULL), m_manager(NULL),
      m_pDetStore(pDetStore), m_pRDBAccess(0)
  {
    MsgStream log(Athena::getMessageSvc(), "MuonGeoModel");
    m_muon = new MuonSystemDescription( "MuonSystem" );
    m_muon->barrelInnerRadius =  4.30*Gaudi::Units::m;
    m_muon->innerRadius       =  0.07*Gaudi::Units::m;
    m_muon->outerRadius       = 13.00*Gaudi::Units::m;
    m_muon->endcapFrontFace   =  6.74*Gaudi::Units::m;
    m_muon->length            = 22.03*Gaudi::Units::m;
    m_muon->barreLength       =  6.53*Gaudi::Units::m; 
    m_muon->barrelInterRadius =  3.83*Gaudi::Units::m;
    m_muon->extraZ = 12.9*Gaudi::Units::m;
    m_muon->extraR = 12.5*Gaudi::Units::m;

    m_selectedStations = std::vector<std::string>(0);
    m_selectedStEta    = std::vector<int>(0);
    m_selectedStPhi    = std::vector<int>(0);

    m_enableFineClashFixing = 0;
  
    log<<MSG::INFO<<"MuonDetectorFactory - constructor "<<" MuonSystem OuterRadius "<< m_muon->outerRadius
       <<" Length "<< m_muon->length <<endmsg;
    //std::cerr<<"MuonDetectorFactory - constructor/// size of vectors "<<m_selectedStations.size()<<" "<<m_selectedStEta.size()<<" "<<m_selectedStPhi.size()<<std::endl;
  }

  MuonDetectorFactory001::~MuonDetectorFactory001() 
  {
    delete m_muon;
  }

  MuonDetectorManager* MuonDetectorFactory001::getDetectorManager() const 
  {
    return m_manager;
  }

  void MuonDetectorFactory001::create( GeoPhysVol* world )
  {
    MsgStream log(Athena::getMessageSvc(), "MuGM:MuonFactory");

    int mem  = 0;
    float cpu  = 0;
    int umem  = 0;
    float ucpu  = 0;
    std::ofstream geoModelStats;
    if (m_dumpMemoryBreakDown)
      {
	geoModelStats.open("MuonGeoModelStatistics_MuonDetectorFactory");
    
    
	mem  = GeoPerfUtils::getMem();
	cpu  = int(GeoPerfUtils::getCpu()/100.);
    
       
	geoModelStats <<"At MuonDetectorFactory::create entry point: \t SZ= " <<mem << " Kb \t Time = " << cpu << " seconds " << std::endl; 

	umem = GeoPerfUtils::getMem();
	ucpu = int(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorFactory::resetting to 0:     \t SZ= " <<umem-mem << " Kb \t Time = " << ucpu-cpu << " seconds " << std::endl;
	mem = umem;
	cpu = ucpu;
      }
    
    
    if (!m_manager)  m_manager = new MuonDetectorManager();
    // std::cout<<" MuonDetectorFactory001::create m_layout = <"<<m_layout<<">"<<std::endl;
    
    // check consistency of flags coming from the tool
    m_includeCutouts = 1;
    m_includeCutoutsBog = 1;
    log<<MSG::INFO<<"MuonLayout set to <"<<m_layout<<"> = Development version for DC3 - infrastructures "
       <<endmsg;
    log<<MSG::INFO<<"                   BOG cutouts are activated "<<m_includeCutoutsBog
       <<" , all other cutouts are disabled "<<m_includeCutouts<<endmsg;

    m_manager->setCachingFlag(m_caching);
    m_manager->setCacheFillingFlag(m_cacheFillingFlag);
    m_manager->setControlAlinesFlag(m_controlAlines);
    // set here the flag defining the geometry granularity
    // minimalgeo = 1 => The geo tree is built up to the Detector Level (Full PhysVol)
    //                     no internal structure of the Detector is built
    // minimalgeo = 0 => The geo tree is built in full details 
    m_manager->setMinimalGeoFlag(m_minimalGeoFlag);
    m_manager->setMdtDeformationFlag(m_mdtDeformationFlag);
    m_manager->setMdtAsBuiltParamsFlag(m_mdtAsBuiltParaFlag);
    if (m_controlCscIntAlines == 0) m_manager->setApplyCscIntAlignment(false);
    else m_manager->setApplyCscIntAlignment(true);
    m_manager->setCscIlinesFlag(m_controlCscIntAlines);
    m_manager->setCscFromGM(m_useCscIntAlinesFromGM);
    m_manager->setGeometryVersion(m_layout);
    m_manager->set_DBMuonVersion(m_DBMuonVersion);
    log<<MSG::INFO<<"Manager created for geometry version "
       <<m_manager->geometryVersion()<<" from DB MuonVersion <"<<m_manager->get_DBMuonVersion()<<">"<<endmsg;
  
    // here create the MYSQL singleton and assign to it the geometry version
    MYSQL *mysql=  MYSQL::GetPointer();	
    mysql->setGeometryVersion(m_layout);
    mysql->set_amdb_from_RDB(m_rdb == 1);
    mysql->set_DBMuonVersion(m_DBMuonVersion);
    log<<MSG::INFO<<"Mysql helper class created here for geometry version "
       <<mysql->getGeometryVersion()<<" from DB MuonVersion <"<<mysql->get_DBMuonVersion()<<">"<<endmsg;
        
  
    StatusCode sc =StatusCode::SUCCESS;

    //     if (m_idhfromconverters != 0)
    //     {

    const MdtIdHelper*  mdtidh = nullptr;
    sc = m_pDetStore->retrieve(mdtidh,"MDTIDHELPER");
    if (sc.isFailure())log<<MSG::ERROR<<" not found MDT "<<endmsg;
    else log<<MSG::INFO<<"MDTIDHELPER retrieved from DetStore"<<endmsg;
    m_manager->set_mdtIdHelper(mdtidh);
    const RpcIdHelper* rpcidh = nullptr;
    sc = m_pDetStore->retrieve(rpcidh,"RPCIDHELPER");
    if (sc.isFailure() )log<<MSG::ERROR<<" not found RPC "<<endmsg;
    else log<<MSG::INFO<<"RPCIDHELPER retrieved from DetStore"<<endmsg;
    m_manager->set_rpcIdHelper(rpcidh);
    const TgcIdHelper* tgcidh = nullptr;
    sc = m_pDetStore->retrieve(tgcidh,"TGCIDHELPER");
    if (sc.isFailure() )log<<MSG::ERROR<<" not found TGC "<<endmsg;
    else log<<MSG::INFO<<"TGCIDHELPER retrieved from DetStore"<<endmsg;
    m_manager->set_tgcIdHelper(tgcidh);
    const CscIdHelper* cscidh = nullptr;
    sc = m_pDetStore->retrieve(cscidh,"CSCIDHELPER");
    if (sc.isFailure() )log<<MSG::ERROR<<" not found CSC "<<endmsg;
    else log<<MSG::INFO<<"CSCIDHELPER retrieved from DetStore"<<endmsg;
    m_manager->set_cscIdHelper(cscidh);

    //for nSW
    const sTgcIdHelper* stgcidh = nullptr;
    sc = m_pDetStore->retrieve(stgcidh,"STGCIDHELPER");
    if (sc.isFailure() )log<<MSG::ERROR<<" not found sTGC "<<endmsg;
    else log<<MSG::INFO<<"STGCIDHELPER retrieved from DetStore"<<endmsg;
    m_manager->set_stgcIdHelper(stgcidh);

    const MmIdHelper* mmidh = nullptr;
    sc = m_pDetStore->retrieve(mmidh,"MMIDHELPER");
    if (sc.isFailure() )log<<MSG::ERROR<<" not found MicroMegas "<<endmsg;
    else log<<MSG::INFO<<"MMIDHELPER retrieved from DetStore"<<endmsg;
    m_manager->set_mmIdHelper(mmidh);



    //     }
    //     else 
    //     {

    // // hand-made IdHelper Initialization 
    // // MdtIdHelper
    // bool mdtidhInitialized=false;
    // MdtIdHelper *mdtidh = new MdtIdHelper;
    // const IdDictManager * idDictMgr = 0; 
    // sc = m_pDetStore->retrieve(idDictMgr, "IdDict");
    // if (sc.isFailure()) {
    //   log<<MSG::ERROR << " Could not get IdDictManager !" << endmsg;
    //   throw std::runtime_error("Error in MuonDetectorFactory, could not get IdDictManager");
    // }
    
    
    // //  if (StatusCode::SUCCESS == sc) {
    // if (idDictMgr) {
    //   if (!idDictMgr->initializeHelper(*mdtidh)) { // Returns 1 if there is a problem
    //     mdtidhInitialized=true;
    //     log<<MSG::INFO << " MdtIdHelper has been initialised! "
    //        << endmsg;
    //   }
    //   else 
    //   {
    //     log<<MSG::WARNING<<" IdDictManager found BUT initializeHelper ="
    //        <<idDictMgr->initializeHelper(*mdtidh)<<endmsg;
    //   }
    // }
    // else log<<MSG::WARNING<<" IdDictManager not found in the Detector Store"<<endmsg;
    // //  }
    // //  else log<<MSG::ERROR<<" FAILURE:: IdDictManager not found in the Detector Store"
    // //<<endmsg;
    // if (!mdtidhInitialized) {
    //   log<<MSG::ERROR << " Cannot initialize MdtIdHelper ! "
    //      << endmsg;
    //   delete mdtidh;
    //   mdtidh=NULL;
    //   throw std::runtime_error("Error in MuonDetectorFactory, cannot initialize MdtIdHelper");
    // }
    // m_manager->set_mdtIdHelper(mdtidh);
    
    // // CscIdHelper
    // bool cscidhInitialized=false;
    // CscIdHelper *cscidh = new CscIdHelper;
    // if (!idDictMgr->initializeHelper(*cscidh)) { // Returns 1 if there is a problem
    //   cscidhInitialized=true;
    //   log<<MSG::INFO << " CscIdHelper has been initialised! "
    //      << endmsg;
    // }
    // if (!cscidhInitialized) {
    //   log<<MSG::ERROR << " Cannot initialize CscIdHelper ! " << endmsg;
    //   delete cscidh;
    //   cscidh=NULL;
    //   throw std::runtime_error("Error in MuonDetectorFactory, cannot initialize CscIdHelper");
    //   return;
    // }  
    // m_manager->set_cscIdHelper(cscidh);
  
    // // RpcIdHelper
    // bool rpcidhInitialized=false;
    // RpcIdHelper *rpcidh = new RpcIdHelper;
    // if (!idDictMgr->initializeHelper(*rpcidh)) { // Returns 1 if there is a problem
    //   rpcidhInitialized=true;
    //   log<<MSG::INFO << " RpcIdHelper has been initialised! " << endmsg;
    // }
    // if (!rpcidhInitialized) {
    //   log<<MSG::ERROR << " Cannot initialize RpcIdHelper ! " << endmsg;
    //   delete rpcidh;
    //   rpcidh=NULL;
    //   throw std::runtime_error("Error in MuonDetectorFactory, cannot initialize RpcIdHelper");
    //   return;
    // }  
    // m_manager->set_rpcIdHelper(rpcidh);

    // // TgcIdHelper
    // bool tgcidhInitialized=false;
    // TgcIdHelper *tgcidh = new TgcIdHelper;
    // if (!idDictMgr->initializeHelper(*tgcidh)) { // Returns 1 if there is a problem
    //   tgcidhInitialized=true;
    //   log<<MSG::INFO << " TgcIdHelper has been initialised! " << endmsg;
    // }
    // if (!tgcidhInitialized) {
    //   log<<MSG::ERROR << " Cannot initialize TgcIdHelper ! " << endmsg;
    //   delete tgcidh;
    //   tgcidh=NULL;
    //   throw std::runtime_error("Error in MuonDetectorFactory, cannot initialize TgcIdHelper");
    //   return;
    // }  
    // m_manager->set_tgcIdHelper(tgcidh);
    // //}
  

    if (m_dumpMemoryBreakDown)
      {
	umem = GeoPerfUtils::getMem();
	ucpu = int(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorFactory::IdHelpers loaded    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
      }
    


    log<<MSG::INFO<<" **************** MuonDetectorFactory001 ************************"<<endmsg;
    log<<  " *** Start building the Muon Geometry Tree **********************"<<endmsg;

    DBReader* dbr = NULL;
    std::string OracleTag =  m_DBkey;
    std::string OracleNode = m_DBnode;

    // Production Rome Final
    // OracleTag = "ATLAS-Rome-Final-00";
    //dbr = new RDBReaderAtlas(m_pDetStore, m_pRDBAccess, OracleTag, OracleNode);

    if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"calling RDBReaderAtlas with m_altAsciiDBMap"<<endmsg;
    dbr = new RDBReaderAtlas(m_pDetStore, m_pRDBAccess, OracleTag, OracleNode, m_dumpAlines, m_useCscIntAlinesFromGM, m_dumpCscIntAlines, &m_altAsciiDBMap);
    RDBReaderAtlas* thisDbr = (RDBReaderAtlas*)dbr;
    thisDbr->setControlCscIntAlines(m_controlCscIntAlines);
    
    // set here the flag deciding whether to include cutouts:
    //m_includeCutouts = 1 => include cutouts
    //m_includeCutouts = 0 => no cutouts
    m_manager->setCutoutsFlag(m_includeCutouts);
    m_manager->setCutoutsBogFlag(m_includeCutoutsBog);
    mysql->setCutoutsBogFlag(m_includeCutoutsBog);
    mysql->setCtbBisFlag(m_includeCtbBis);
    mysql->setControlAlines(m_controlAlines);
    
    dbr->setGeometryVersion(m_layout);
    dbr->setManager(m_manager);
    sc = dbr->ProcessDB();
    if (sc != StatusCode::SUCCESS)
      {
        log << MSG::ERROR<<" FAILURE in DB access; Muon node will not be built"<<endmsg;
        return;
      }
    //release memory
    delete dbr;
    dbr = 0;

    if (m_dumpMemoryBreakDown)
      {
	umem = GeoPerfUtils::getMem();
	ucpu = int(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorFactory::DB read             \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
      }
    


    
    // now store detector-generic-descriptors into the manager
    RPC* r = (RPC*)mysql->GetATechnology("RPC0");
    GenericRPCCache rpcCache;
    rpcCache.stripSeparation = r->stripSeparation;
    rpcCache.stripPanelThickness = r->stripPanelThickness;
    rpcCache.rpcLayerThickness = r->rpcLayerThickness;
    rpcCache.centralSupPanelThickness = r->centralSupPanelThickness;
    rpcCache.GasGapThickness = r->GasGapThickness;
    rpcCache.frontendBoardWidth = r->frontendBoardWidth;
    m_manager->setGenericRpcDescriptor(rpcCache);

    MDT* mdtobj = (MDT*)mysql->GetATechnology("MDT0");
    GenericMDTCache mdtCache;
    mdtCache.innerRadius = mdtobj->innerRadius;
    mdtCache.outerRadius = mdtobj->innerRadius+mdtobj->tubeWallThickness;
    //std::cout<<" mdtCache: innerRadius, outerRadius = "<<mdtCache.innerRadius<<" "<<mdtCache.outerRadius<<" tube wass thickness = "<<mdtobj->tubeWallThickness<<std::endl;
    m_manager->setGenericMdtDescriptor(mdtCache);

    TGC* t = (TGC*)mysql->GetATechnology("TGC0");
    GenericTGCCache tgcCache;
    tgcCache.frame_h = t->frame_h;
    tgcCache.frame_ab = t->frame_ab;
    tgcCache.nlayers = t->nlayers;
    //     std::cout << " TGC SIZE CHECK: nlayers = " << t->nlayers 
    //               << " materials.size() = " << (t->materials).size() << std::endl;
    // no need for the following info in the Generic TGC cache  
    //     for (unsigned int i = 0; i < (t->materials).size(); i++) 
    //     {
    // 	//tgcCache.materials[i] = t->materials[i];
    // 	(tgcCache.materials).push_back(t->materials[i]);
    // 	//tgcCache.positions[i] = t->positions[i];
    // 	(tgcCache.positions).push_back(t->positions[i]);
    // 	//tgcCache.tck[i] = t->tck[i];
    // 	(tgcCache.tck).push_back(t->tck[i]);
    //     }
    m_manager->setGenericTgcDescriptor(tgcCache);

    //    CSC* c = (CSC*)mysql->GetATechnology("CSC0");
    GenericCSCCache cscCache;
    cscCache.dummy1 = 0;
    cscCache.dummy2 = 0;
    m_manager->setGenericCscDescriptor(cscCache);

/*    // TMP DEBUG temporary code to read the as-built parameters from an ASCII file
    {
      AltAsciiDBMap::const_iterator fileit = m_altAsciiDBMap.find("XAMDT");
      if (fileit != m_altAsciiDBMap.end()) {
	const std::string& file = fileit->second;
	std::ifstream fin(file.c_str());
	std::string line;
	MdtAsBuiltPar xPar;
	int count = 0;
	while(getline(fin, line)) {
	  if (line.find("Corr:")==0) {
	    if (!xPar.setFromAscii(line)) {
	      log<< MSG::ERROR
		<< "Unable to parse AsBuilt params from Ascii line: " << line
		<<endmsg;
	    } else {
	      m_manager->storeMdtAsBuiltParams(new MdtAsBuiltPar(xPar));
	      ++count;
	    }
	  }
	}
	log << MSG::INFO << "Parsed AsBuilt parameters: " << count << endmsg;
      }
    } */

    const StoredMaterialManager*  theMaterialManager;
    if ( StatusCode::SUCCESS != m_pDetStore->retrieve( theMaterialManager, "MATERIALS" ) )  
      {
        return;
      }
    else
      log<< MSG::INFO
	 <<" theMaterialManager retrieven successfully from the DetStore"
	 <<endmsg;
  
  
    DetectorElement::setMaterialManager(*theMaterialManager);  
  
    const GeoMaterial* m4 = theMaterialManager->getMaterial( "std::Air" );
    GeoLogVol*  l4;
    GeoPcon* c4 = new GeoPcon( 0, 360*Gaudi::Units::deg );
    //--- --- --- CREATE ENVELOPE --- --- ---
    // First try to get data from the GeomDB
    IRDBRecordset_ptr muonSysRec = m_pRDBAccess->getRecordsetPtr("MuonSystem",OracleTag,OracleNode);
    // -- Next two lines allow to use MuonSystem-00 by default instead of hardwired numbers
    //    even for geometry tags where MuonSystem was not collected 
    if(muonSysRec->size()==0)
      {
	muonSysRec = m_pRDBAccess->getRecordsetPtr("MuonSystem","MuonSystem-00");
	log<< MSG::INFO
	   <<"MuonSystem description from default node in GeomDB, i.e. MuonSystem-00"<<endmsg;
      }
    else log<< MSG::INFO
	    <<"MuonSystem description from OracleTag=<"<<OracleTag<<"> and node=<"<< OracleNode<<">"<<endmsg;
    //
    
    // --- Envelope from DB ....
    if(muonSysRec->size()!=0)
      {
	// Data retrieved
	muonsysIndMap indmap;
	muonsysIndMap::const_iterator iter;
	const IRDBRecord* currentRecord;
        
	// First fill the contents of muonsysIndMap
	for (unsigned int ind=0; ind<muonSysRec->size(); ind++)
	  {
	    int key = (*muonSysRec)[ind]->getInt("PLANE_ID");
	    indmap[key] = ind;
	  }
	
	// Create the polycone
	for(unsigned int ind=0; ind<indmap.size(); ind++)
	  {
	    iter = indmap.find(ind);
                
	    if(iter==indmap.end())
	      throw std::runtime_error("Error in MuonDetectorFactory, missing plane in MuonSystem");
	    else
	      {
		currentRecord = (*muonSysRec)[(*iter).second];
		c4->addPlane(currentRecord->getDouble("ZPLANE"),
			     currentRecord->getDouble("RMIN"),
			     currentRecord->getDouble("RMAX"));
	      }
	  }
      }// ... end if  Envelope from DB ---
    else
      {
	// Muon System node is not present, go for handcoded version
	log<< MSG::INFO
	   <<"MuonSystem description not available in GeomDB - using hard-wired description"<<endmsg;
	
	double ir   = m_muon->barrelInnerRadius;
	double pir  = m_muon->innerRadius;
	double orad = m_muon->outerRadius;
	double l    = m_muon->length;
	double eff  = m_muon->endcapFrontFace;
        
	double extraR = m_muon->extraR;
	double extraZ = m_muon->extraZ;
        
            
	c4->addPlane(     -l, pir, extraR);
	c4->addPlane(-extraZ, pir, extraR);
	c4->addPlane(-extraZ, pir, orad);
        
	c4->addPlane(-eff, pir, orad);
	c4->addPlane(-eff,  ir, orad);
	c4->addPlane(+eff,  ir, orad);
	c4->addPlane(+eff, pir, orad);
        
	c4->addPlane(extraZ, pir, orad);
	c4->addPlane(extraZ, pir, extraR);
	c4->addPlane(     l, pir, extraR);
      }
    l4 = new GeoLogVol( "MuonSys", c4, m4 );
    
    GeoPhysVol* p4 = new GeoPhysVol(l4);
  
    // Cannot (yet) cope with this:
    // G4UserLimits *ul=new G4UserLimits;
    // ul->SetMaxAllowedStep(5*Gaudi::Units::cm);
    // lv->GetLogicalVolume()->SetUserLimits(ul);
  
    m_manager->addTreeTop(p4); // This is the top!
    log << MSG::INFO<<" TreeTop added to the Manager"<<endmsg;
    log << MSG::INFO<<" Muon Layout "<< m_layout << endmsg;

    
    std::vector<std::string> slist;
    if (m_selectedStations.size()<=0) slist.push_back("*");
    else slist = m_selectedStations;
    

    // create the fullphysvol map to allow cloning and save memory
    FPVMAP* savemem = FPVMAP::GetPointer();
  
  
    int nstat_ss = 0;
    int ntpos_ss = 0;
    int npos_ss = 0;
    int nAssemblies = 0;
    if (m_enableFineClashFixing >0)
      log<<MSG::INFO
	 <<"Fine Clash Fixing enabled:  (should be ON/OFF for Simulation/Reconstruction)"<<endmsg;
    else
      log<<MSG::INFO
	 <<"Fine Clash Fixing disabled: (should be ON/OFF for Simulation/Reconstruction)"<<endmsg;

    

    StationSelector sel(slist);
    StationSelector::StationIterator it;
    //  log<<MSG::DEBUG<<" Size of the station list is ???"<<endmsg;
    for ( it = sel.begin(); it != sel.end(); it++ )
      {
        Station *station = (*it).second;
        std::string stname(station->GetName(), 0, 3);
        if (m_selectedStations.size()<=0) 
	  if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE<<"Processing Stations named <"<<station->GetName()<<"> "
					    <<nstat_ss<<" built until now"<<endmsg;

	if ((skip_chambers) && (stname.substr(0,1)!="X")) continue;

        bool isAssembly = false;
        if (station->GetNrOfCutouts() > 0 && stname.substr(0,1) != "T") isAssembly = true;
	// BIR have multilayers of diff. length and overall station volume clashes with toroids    
	if (stname=="BIR") isAssembly = true;
	MuonChamber l(station);  // here is where we start to create a MuonChamber with all readoutelements
        l.setFineClashFixingFlag(m_enableFineClashFixing);

        PositionIterator pit;
        AlignPosIterator apit;
        AlignPos ap;
        npos_ss = 0;
        for ( pit = station->begin(); pit != station->end(); pit++ )
	  {
            int zi = (*pit).second.zindex;
            int fi = (*pit).second.phiindex;
            int sign = 1;
            if(zi) sign = zi/abs(zi);

            // if there's a selection of eta location, check if this chamber is selected
            if (m_selectedStEta.size() >0) 
	      {
                bool selectedEta = false;
                for (std::vector<int>::iterator i=m_selectedStEta.begin(); i<m_selectedStEta.end(); i++)
		  {
                    if (zi == *i)
		      {
                        selectedEta = true;
                        break;
		      }
		  }
                if (!selectedEta) continue;
	      }
            // if there's a selection of phi location, check if this chamber is selected
            if (m_selectedStPhi.size() >0) 
	      {
                bool selectedPhi = false;
                for (std::vector<int>::iterator i=m_selectedStPhi.begin(); i<m_selectedStPhi.end(); i++)
		  {
                    if ((fi+1) == *i)
		      {
                        selectedPhi = true;
                        break;
		      }
		  }
                if (!selectedPhi) continue;
	      }

            // total number of stations positioned in the layout
            ntpos_ss++;
            // total number of locations for this chamber 
            npos_ss++;
            
            if ((m_selectedStPhi.size() + m_selectedStEta.size()) >0)
	      log<<MSG::INFO<<"Build selected Station <"<<station->GetName()<<"> at Jzz = "<<zi<<" Jff = "<<fi+1<<" ******* "
		 <<nstat_ss<<" stat.types built until now"<<endmsg;
            

            // here define GeoNameTag 
            GeoNameTag   *nm = new GeoNameTag( station->GetName() + "_station" );

            // here build the physical volume (tree) associated to the chamber 
            bool is_mirrored =  ((*pit).second).isMirrored;
            int mirsign = 0;
            if ( zi<0 && !is_mirrored ) mirsign = 1;
            if (stname=="CSL") isAssembly = true; 
	    //CSL because coffin shape of the station mother volume
            GeoVPhysVol *pv = l.build(m_manager, zi, fi, is_mirrored, isAssembly);
            if (isAssembly) nAssemblies ++;

            // here define GeoIdentifierTag 
            if (useAssemblies || isAssembly) 
	      {
                int sideC = 0;
                if ( zi<0 ) sideC = 1;
                int geoid = (sideC*10000 + mirsign*1000 + abs(zi)*100 + fi+1)*100000;
                if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Adding Station with nameTag=<"<<station->GetName() + "_station"
						<<"> and geoId "<<geoid<<endmsg;
                p4->add(new GeoIdentifierTag (geoid));
	      }
            else 
	      {
                int geoid = (mirsign*1000 + abs(zi)*100 + fi+1)*sign;
                p4->add(new GeoIdentifierTag (geoid));
                if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG<<"Adding Station with nameTag=<"<<station->GetName() + "_station"
						<<"> and geoId "<<geoid<<" for zi/fi = "<<zi<<"/"<<fi<<endmsg;
	      }            

            // here define the GeoAlignableTransform associated to the chamber
            // nominal transform first 
            GeoAlignableTransform *xf = new GeoAlignableTransform(station->getNominalTransform( (*pit).second) );

            // add tag, transform and physicalvolume associated to the chamber to the mother-volume
            p4->add(nm);
            p4->add(xf);
            p4->add(pv);

	    // alignment issues and readout geometry for station
            MuonStation* mst = m_manager->getMuonStation (station->GetName(), zi, fi+1);
            if (mst == NULL) 
	      {
		log<<MSG::WARNING<<"For Station with nameTag=<"<<station->GetName() 
		   <<"> at zi/fi = "<<zi<<"/"<<fi<<" no MuonStation found => no possibility to align"<<endmsg;
		continue;
	      }
	    mst->setTransform(xf);
	    GeoTrf::Transform3D tsz_to_szt = GeoTrf::RotateZ3D(-90*CLHEP::deg)*GeoTrf::RotateY3D(-90*CLHEP::deg);
	    mst->setNativeToAmdbLRS( Amg::EigenTransformToCLHEP(tsz_to_szt * station->native_to_tsz_frame( (*pit).second )) );
	    mst->setNominalAmdbLRSToGlobal( Amg::EigenTransformToCLHEP(station->tsz_to_global_frame( (*pit).second ) * tsz_to_szt.inverse()) );
            
            // find correct alignment information for this position
            // xf->setDelta(DummyAline); // just in case we don't find one

            // apit = station->FindAlignPos(zi,fi);
	    // number of A-lines for this station
	    int nAlines = station->CountAlignPos(zi,fi);
	    //nAlines=-1; 
	    if (nAlines==0)
	      {
		if (log.level()<=MSG::DEBUG) log<<MSG::DEBUG <<"No A-lines(-> AlignedPositions) for this station in the static Geometry DB; setting to def."<<endmsg;
		ap.tras=ap.traz=ap.trat=ap.rots=ap.rotz=ap.rott=0.;
		ap.jobindex=0;
		mst->setDelta_fromAline( ap.tras, ap.traz, ap.trat, ap.rots, ap.rotz, ap.rott );
	      }
	    else if ( nAlines >0 )
	      {
                //std::cout<<"There's a delta transform to be applied"<<std::endl;
		AlignPosIterator alast;
		AlignPosIterator afirst =station->getFirstAlignPosInRange(zi,fi, alast);
	        for (AlignPosIterator acurrent=afirst; acurrent!=alast; ++acurrent)
		  {
		    ap = acurrent->second;
		    //                ap.rots = 0.001356;
		    //                ap.rotz =-0.001113;
		    //                ap.rott = 0.001067;
		    if (ap.phiindex != fi || ap.zindex != zi)
		      {
			log<<MSG::ERROR<<"Inconsistent AlignedPosition found in the static Geometry DB: aligPos.fi, zi = "<< ap.phiindex<<", "<<ap.zindex
			   <<" for station "<<station->GetName()<<" at fi/zi = "<<fi<<"/"<<zi
			   <<" AlignPos indices fi/zi "<<fi<<"/"<<zi<<endmsg;
		      }
		    // 
		    if (ap.jobindex==0) 
		      {
			// station
			if (log.level()<=MSG::DEBUG) {
			  log<<MSG::DEBUG<<"Going to set delta from A-line for station"<<stname<<" at zi/fi "<<zi<<"/"<<fi<<endmsg;
			  log<<MSG::DEBUG<<"A-line is: "
			     <<ap.tras<<" "<<ap.traz<<" "<<ap.trat<<" "
			     <<ap.rots<<" "<<ap.rotz<<" "<<ap.rott<<endmsg;
			}
			mst->setDelta_fromAline( ap.tras, ap.traz, ap.trat, ap.rots, ap.rotz, ap.rott );
		      }
		    else 
		      {
			// station component
			if (log.level()<=MSG::DEBUG) {
			  log<<MSG::DEBUG<<"Going to set delta from A-line for component "<<ap.jobindex
			     <<" of station "<<stname<<" at zi/fi "<<zi<<"/"<<fi<<endmsg;
			  log<<MSG::DEBUG<<"A-line is: "
			     <<ap.tras<<" "<<ap.traz<<" "<<ap.trat<<" "
			     <<ap.rots<<" "<<ap.rotz<<" "<<ap.rott<<endmsg;
			}
			mst->setDelta_fromAline_forComp(ap.jobindex, ap.tras, ap.traz, ap.trat, ap.rots, ap.rotz, ap.rott );
		      }
		  }
	      }

	    //             if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE << "From the Factory:: Total transformation for " << stname << ":"
	    //                << endmsg <<
	    //                 (xf->getTransform())[0][0] << " " <<
	    //                 (xf->getTransform())[0][1] << " " <<
	    //                 (xf->getTransform())[0][2] << " " <<
	    //                 (xf->getTransform())[0][3] << " " << endmsg <<
	    //                 (xf->getTransform())[1][0] << " " <<
	    //                 (xf->getTransform())[1][1] << " " <<
	    //                 (xf->getTransform())[1][2] << " " <<
	    //                 (xf->getTransform())[1][3] << " " << endmsg <<
	    //                 (xf->getTransform())[2][0] << " " <<
	    //                 (xf->getTransform())[2][1] << " " <<
	    //                 (xf->getTransform())[2][2] << " " <<
	    //                 (xf->getTransform())[2][3] << " " << endmsg;
            // get the MuonStation from the manager; store the relevant transforms;
            // let the MuonStation compute the Delta of the GeoAlignableTransform (new style)
            // MuonStation* mst = m_manager->getMuonStation (station->GetName(), zi, fi+1);
            // if (mst != NULL) {
            //     mst->setTransform(xf);
            //     GeoTrf::Transform3D tsz_to_szt = GeoTrf::Transform3D(HepGeom::Point3D<double>(1.,0.,0.),HepGeom::Point3D<double>(0.,1.,0.),
            //                                                HepGeom::Point3D<double>(0.,0.,1.),HepGeom::Point3D<double>(0.,0.,1.),
            //                                                HepGeom::Point3D<double>(1.,0.,0.),HepGeom::Point3D<double>(0.,1.,0.));
            //     mst->setNativeToAmdbLRS( tsz_to_szt * station->native_to_tsz_frame( (*pit).second ) );
            //     mst->setNominalAmdbLRSToGlobal( station->tsz_to_global_frame( (*pit).second ) * tsz_to_szt.inverse() );
	    //                 if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE << "From the Factory:: Before setting delta to the MuonStation " << stname << ":"
	    //                    << endmsg <<
	    //                     (xf->getTransform())[0][0] << " " <<
	    //                     (xf->getTransform())[0][1] << " " <<
	    //                     (xf->getTransform())[0][2] << " " <<
	    //                     (xf->getTransform())[0][3] << " " << endmsg <<
	    //                     (xf->getTransform())[1][0] << " " <<
	    //                     (xf->getTransform())[1][1] << " " <<
	    //                     (xf->getTransform())[1][2] << " " <<
	    //                     (xf->getTransform())[1][3] << " " << endmsg <<
	    //                     (xf->getTransform())[2][0] << " " <<
	    //                     (xf->getTransform())[2][1] << " " <<
	    //                     (xf->getTransform())[2][2] << " " <<
	    //                     (xf->getTransform())[2][3] << " " << endmsg;
	    // GeoTrf::Transform3D Delta_amdb_frame = tsz_to_szt*station->getDeltaTransform_tszFrame(ap)*tsz_to_szt.inverse();
	    // mst->setDeltaAmdbLRS(  Delta_amdb_frame );
	    //                 if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE << "From the Factory:: Delta(amdb) transformation for " << stname << ":"
	    //                    << endmsg <<
	    //                     (Delta_amdb_frame)[0][0] << " " <<
	    //                     (Delta_amdb_frame)[0][1] << " " <<
	    //                     (Delta_amdb_frame)[0][2] << " " <<
	    //                     (Delta_amdb_frame)[0][3] << " " << endmsg <<
	    //                     (Delta_amdb_frame)[1][0] << " " <<
	    //                     (Delta_amdb_frame)[1][1] << " " <<
	    //                     (Delta_amdb_frame)[1][2] << " " <<
	    //                     (Delta_amdb_frame)[1][3] << " " << endmsg <<
	    //                     (Delta_amdb_frame)[2][0] << " " <<
	    //                     (Delta_amdb_frame)[2][1] << " " <<
	    //                     (Delta_amdb_frame)[2][2] << " " <<
	    //                     (Delta_amdb_frame)[2][3] << " " << endmsg;
	    //                 //now get and check the full transform
	    //                 if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE << "From MuonStation:: Total transformation for " << stname << ":"
	    //                    << endmsg <<
	    //                     (xf->getTransform())[0][0] << " " <<
	    //                     (xf->getTransform())[0][1] << " " <<
	    //                     (xf->getTransform())[0][2] << " " <<
	    //                     (xf->getTransform())[0][3] << " " << endmsg <<
	    //                     (xf->getTransform())[1][0] << " " <<
	    //                     (xf->getTransform())[1][1] << " " <<
	    //                     (xf->getTransform())[1][2] << " " <<
	    //                     (xf->getTransform())[1][3] << " " << endmsg <<
	    //                     (xf->getTransform())[2][0] << " " <<
	    //                     (xf->getTransform())[2][1] << " " <<
	    //                     (xf->getTransform())[2][2] << " " <<
	    //                     (xf->getTransform())[2][3] << " " << endmsg;
	    //now get and check the full transform after updating the delta directly with a-line
	    //std::cout<<setiosflags(std::ios::fixed) << std::setprecision(6) << std::setw(12);
	    //std::cout<<"A-lines from DB to MuonStation: "<<ap.tras<<" "<<ap.traz<<" "<<ap.trat<<" "<<ap.rots<<" "<<ap.rotz<<" "<<ap.rott<<std::endl;
		
	    //                 if (log.level()<=MSG::VERBOSE) log<<MSG::VERBOSE << "From MuonStation (A line):: Total transformation for " << stname << ":"
	    //                    << endmsg <<
	    //                     (xf->getTransform())[0][0] << " " <<
	    //                     (xf->getTransform())[0][1] << " " <<
	    //                     (xf->getTransform())[0][2] << " " <<
	    //                     (xf->getTransform())[0][3] << " " << endmsg <<
	    //                     (xf->getTransform())[1][0] << " " <<
	    //                     (xf->getTransform())[1][1] << " " <<
	    //                     (xf->getTransform())[1][2] << " " <<
	    //                     (xf->getTransform())[1][3] << " " << endmsg <<
	    //                     (xf->getTransform())[2][0] << " " <<
	    //                     (xf->getTransform())[2][1] << " " <<
	    //                     (xf->getTransform())[2][2] << " " <<
	    //                     (xf->getTransform())[2][3] << " " << endmsg;
	
            //m_manager->addStation(xf,nm); //ss 19-02-2007 not needed anymore 
	  }//end loop on positions 
        //number of stations realised in the layout 
        if (npos_ss >0) nstat_ss ++;
      }

    log<<MSG::INFO<<" **************** MuonDetectorFactory001 ****************************"<<endmsg
       <<" *** The Muon Chamber Geometry Tree is built with "<<endmsg
       <<" *** "<<p4->getNChildVols()<<" child volumes "<<endmsg
       <<" *** "<<savemem->NDetectors()<<" independent elements and "<<endmsg
       <<" *** "<<savemem->NDetectorsReused()<<" elements cloned or shared "<<endmsg
       <<" *** "<<nstat_ss<<" kinds of stations"<<endmsg
       <<" *** "<<ntpos_ss<<" stations with alignable transforms"<<endmsg
       <<" *** "<<nAssemblies<<" stations are described as Assemblies"<<endmsg
       <<" *** "<<m_manager->nMuonStation()<<" MuonStations "<<endmsg
       <<" *** \t "<<m_manager->nMdtRE()<<" MDT Readout Elements \t "
       <<m_manager->nMdtDE()<<" MDT Detector Elements "<<endmsg
       <<" *** \t "<<m_manager->nCscRE()<<" CSC Readout Elements \t "
       <<m_manager->nCscDE()<<" CSC Detector Elements "<<endmsg
       <<" *** \t "<<m_manager->nRpcRE()<<" RPC Readout Elements \t "
       <<m_manager->nRpcDE()<<" RPC Detector Elements "<<endmsg
       <<" *** \t "<<m_manager->nTgcRE()<<" TGC Readout Elements \t "
       <<m_manager->nTgcDE()<<" TGC Detector Elements "<<endmsg
       <<" ********************************************************************"<<endmsg;

    if (m_dumpMemoryBreakDown)
      {
	umem = GeoPerfUtils::getMem();
	ucpu = int(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorFactory::active geo done     \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
      }
    


    
    // delete the fullphysvol map
    delete savemem;
    savemem = NULL;

    // delete the station and technology map
    delete mysql;
    mysql = NULL;


    if (m_dumpMemoryBreakDown)
      {
	umem = GeoPerfUtils::getMem();
	ucpu = int(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorFactory::released access.mem \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
      }
    


    
    GeoNameTag *ntg = new GeoNameTag( "Muon" );
    world->add(ntg);
    world->add(p4);	  
    

    log << MSG::INFO
        << " *** Inert Material built according to DB switches and config. ****** "<< endmsg;
    log << MSG::INFO
        << " *** The Muon Geometry Tree has " << p4->getNChildVols() << " child vol.s in total ********"<< endmsg
        << " ********************************************************************\n"<< endmsg;
  
    if (m_dumpMemoryBreakDown)
      {
	umem = GeoPerfUtils::getMem();
	ucpu = int(GeoPerfUtils::getCpu()/100.);
	geoModelStats <<"At MuonDetectorFactory::inert  geo done     \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	mem = umem;
	cpu = ucpu;
    
	geoModelStats.close();
      }
    
  }

} // namespace MuonGM

