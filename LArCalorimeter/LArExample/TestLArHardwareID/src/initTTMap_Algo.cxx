/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include "TestLArHardwareID/initTTMap_Algo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/LArID_Exception.h"
//#include "LArIdentifier/LArIdManager.h"
//#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/LArTTCellMap.h"

/********************************************************/
initTTMap_Algo::initTTMap_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  Algorithm( name , pSvcLocator) ,
  m_initFromFile("OFF"),
  m_dumpMap(false),
  m_detStore(0),
  m_cablingSvc(0),
  m_triggertowerSvc(0),
  m_lvl1Helper(0),
  m_emHelper(0),
  m_hecHelper(0),
  m_fcalHelper(0)
  // ,m_onlHelper(0)
{

  // TTmap Offline Property:
  // ======================
  declareProperty("initFromFile", m_initFromFile );
  if( m_initFromFile != "ON" && m_initFromFile != "OFF" )    {
    m_initFromFile = "OFF";
  } 
      
  declareProperty("dumpMap", m_dumpMap );
      
}

initTTMap_Algo::~initTTMap_Algo()
{
}

// ==============================================================
StatusCode initTTMap_Algo::initialize(){
// ==============================================================
  MsgStream log( messageService(), name() );
  log << MSG::INFO << " initializing " << endreq;

  StatusCode sc ;  
  IToolSvc* toolSvc;
  StatusCode status = service( "ToolSvc",toolSvc  );
 
  if(status.isSuccess()) 
    {
      StatusCode sc = toolSvc->retrieveTool("LArCablingService", m_cablingSvc); 
      if(sc != StatusCode::SUCCESS) {
	log << MSG::ERROR << "initialize() failed retrieving LArCablingService" << endreq;
	return StatusCode::FAILURE;
      }
      else
	{
	  log << MSG::DEBUG << "initialize() successfully retrieved LArCablingService" << endreq;
	}
    }
  else 
    {
      log << MSG::ERROR << "initialize() failed locating ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }

  // CaloTriggerTowerService
  // ----------------------
  if(status.isSuccess()) 
    {
      StatusCode sc = toolSvc->retrieveTool("CaloTriggerTowerService", m_triggertowerSvc); 
      if(sc != StatusCode::SUCCESS) {
	log << MSG::ERROR << "initialize() failed retrieving CaloTriggerTowerService" << endreq;
	return StatusCode::FAILURE;
      }
      else
	{
	  log << MSG::DEBUG << "initialize() successfully retrieved CaloTriggerTowerService" << endreq;
	}
    }
  else 
    {
      log << MSG::ERROR << "initialize() failed locating ToolSvc" << endreq;
      return StatusCode::FAILURE;
    }




  // retrieve LArId and CaloId managers and helpers from det store
  // -------------------------------------------------------------
  sc = service( "DetectorStore", m_detStore );
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to locate DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Successfully located DetectorStore" << endreq;
  }	

  /*
  const LArIdManager*	larIdMgr;
  sc = m_detStore->retrieve(larIdMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve LArIdManager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Successfully retrieved LArIdManager from DetectorStore" << endreq;
  
    const LArOnlineID* onlId = LArIdMgr->getOnlineID();
    if (!onlId) {
      log << MSG::ERROR << "Could not access lar online ID helper" << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::INFO << "Successfully accessed lar online ID helper" << endreq;
      m_onlHelper = onlId ;
    }
  }	
  */

  const CaloIdManager*	caloIdMgr;
  sc = m_detStore->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve CaloIdManager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Successfully retrieved CaloIdManager from DetectorStore" << endreq;
  
    const CaloLVL1_ID* caloId = caloIdMgr->getLVL1_ID();
    if (!caloId) {
      log << MSG::ERROR << "Could not access calolvl1 ID helper" << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::INFO << "Successfully accessed calolvl1 ID helper" << endreq;
      m_lvl1Helper = caloId ;
    }
    
    const LArEM_ID* emId = caloIdMgr->getEM_ID();
    if (!emId) {
      log << MSG::ERROR << "Could not access lar EM ID helper" << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::INFO << "Successfully accessed lar EM ID helper" << endreq;
      m_emHelper=emId;
    }
    
    const LArHEC_ID* hecId = caloIdMgr->getHEC_ID();
    if (!hecId) {
      log << MSG::ERROR << "Could not access lar HEC ID helper" << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::INFO << "Successfully accessed lar HEC ID helper" << endreq;
      m_hecHelper=hecId;
    }
    
    const LArFCAL_ID* fcalId = caloIdMgr->getFCAL_ID();
    if (!fcalId) {
      log << MSG::ERROR << "Could not access lar FCAL ID helper" << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::INFO << "Successfully accessed lar FCAL ID helper" << endreq;
      m_fcalHelper=fcalId;
    }
    
  }	
  
  return sc;
}


// ====================================================================================
StatusCode initTTMap_Algo::execute(){
// ====================================================================================

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "=> initTTMap_Algo::Executing " << endreq;

  StatusCode sc = initMap();

  if(sc!= StatusCode::FAILURE) {
    sc = testStruct();
  } else {
    log << MSG::ERROR << "initTTMap_Algo:: failed init map " << endreq;
  }

  return sc ;
}
// ====================================================================================
StatusCode initTTMap_Algo::initMap(){
// ====================================================================================

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "=> initTTMap_Algo::initMap " << endreq;

  std::vector<Identifier>::const_iterator itTt  = m_lvl1Helper->layer_begin() ;
  std::vector<Identifier>::const_iterator itEnd = m_lvl1Helper->layer_end() ;

  int cell_det = 0;
  int cell_pn = 0;
  int cell_reg = 0;
  int cell_samp = 0;
  int cell_eta = 0;
  int cell_phi = 0;

  int nCell=0;
  int nNE=0;
  int nSkip=0;
  int nDisc=0;
  LArTTCell d;

  // ....... init EM and HEC from hard coded mapping in LArCablingService
  // ....... loop on Trigger Towers

  log << MSG::DEBUG
      << " Starting loop on Trigger Towers " 
      << endreq;
  for(; itTt!=itEnd;++itTt){
	    
    Identifier layerId = (*itTt) ;
    
    //
    // ........ skip Tile cal
    //
    if(!m_lvl1Helper->is_tile(layerId)){  

      int l1_pn = m_lvl1Helper->pos_neg_z(layerId); 
      int l1_samp = m_lvl1Helper->sampling(layerId); 
      int l1_reg = m_lvl1Helper->region(layerId);
      int l1_eta = m_lvl1Helper->eta(layerId);
      int l1_phi = m_lvl1Helper->phi(layerId);
      int l1_lay = m_lvl1Helper->layer(layerId);
      //
      // .... conversion to an 'online' TT identifier
      //
      // 
      //... fill a vector with online identifier of cells belonging to this tower (with layer info)
      //
      std::vector<Identifier> channelIdVec = m_triggertowerSvc->createCellIDvecLayer(layerId);
      //
      // .... loop on all LAr online channels belonging to the trigger tower (with layer info) 
      //
      if(channelIdVec.size()==0) {
	log<<MSG::VERBOSE<<"non existing extented TT " << m_lvl1Helper->show_to_string(layerId)<<endreq ;
	nNE++;
      } else {
	for (unsigned int ichan=0;ichan<channelIdVec.size();ichan++){	
	  //
	  // .... convert to cell offline identifier
	  //
	  Identifier cellId = channelIdVec[ichan];
	  nCell++;
	  bool fcal=false;
	  if(m_emHelper->is_lar_em(cellId)) {
	    log<<MSG::VERBOSE<<"EM cell " << m_lvl1Helper->show_to_string(cellId)<<endreq ;
	    cell_det = 0;
	    cell_pn = m_emHelper->barrel_ec(cellId);
	    cell_samp = m_emHelper->sampling(cellId);
	    cell_reg = m_emHelper->region(cellId);
	    cell_eta = m_emHelper->eta(cellId);
	    cell_phi = m_emHelper->phi(cellId);
	  }
	  else if(m_emHelper->is_lar_hec(cellId)) {
	    log<<MSG::VERBOSE<<"HEC cell " << m_lvl1Helper->show_to_string(cellId)<<endreq ;
	    cell_det = 1;
	    cell_pn = m_hecHelper->pos_neg(cellId);
	    cell_samp = m_hecHelper->sampling(cellId);
	    cell_reg = m_hecHelper->region(cellId);
	    cell_eta = m_hecHelper->eta(cellId);
	    cell_phi = m_hecHelper->phi(cellId);
	  }
	  else if(m_emHelper->is_lar_fcal(cellId)) {
	    log<<MSG::VERBOSE<<"FCAL cell " << m_lvl1Helper->show_to_string(cellId)<<endreq ;
	    fcal=true;
	    cell_det = 2;
	    cell_samp = 0;
	    // old bug concerning 2 disconnected channels (in hardcoded mapping)
	    if(m_fcalHelper->is_connected(cellId)) {
	      // FCAL sample==region
	      cell_pn = m_fcalHelper->pos_neg(cellId);
	      cell_reg = m_fcalHelper->module(cellId);
	      cell_eta = m_fcalHelper->eta(cellId);
	      cell_phi = m_fcalHelper->phi(cellId);
	    } else {
	      nDisc++;
	      cell_pn = -2;
	      cell_reg = 3;
	      cell_eta = 15;
	      cell_phi = 5;
	      if(nDisc==2) {cell_phi = 13;}
	      
	      log<<MSG::DEBUG << "disc chan= " 
		 << m_fcalHelper->show_to_string(cellId)
		 << endreq;
	    }
	  } else {
	    log<<MSG::ERROR<< "Cell not in EM, nor HEC, nor FCAL: "
	       << m_emHelper->show_to_string(cellId) 
	       << " from Trigger Tower " 
	       << m_emHelper->show_to_string(layerId) 
	       << endreq ;
	  }
	  
	  if(m_initFromFile == "OFF" || !fcal) {
	    // fill map only if not FCAL, 
	    // or FCAL not supposed to be init by Gerald's file
	    LArTTCell_t t ;
	    
	    t.det = cell_det;
	    t.pn  = cell_pn;
	    t.sample = cell_samp;
	    t.region = cell_reg;
	    t.eta = cell_eta; 
	    t.phi = cell_phi;
	    
	    t.tpn  = l1_pn;
	    t.tsample = l1_samp;
	    t.tregion = l1_reg;
	    t.teta = l1_eta      ; 
	    t.tphi = l1_phi     ;
	    t.layer = l1_lay     ;
	  
	    d.push_back(t); 
	  } else {
	    nSkip++;
	  }
	}
      }
    }
  }
  log<<MSG::INFO<< "number of cells found, skipped= " << nCell << " " << nSkip << endreq ;
  log<<MSG::INFO<< "number of Non Existing Extented TT= " << nNE << endreq ;

  if(m_initFromFile == "ON") {
    // the FCAL part is read from Gerald Oakham's file

    std::string fcalmapname=PathResolver::find_file ("FCal-online-map.txt", "DATAPATH");
    if (fcalmapname == "") {
      log << MSG::ERROR << "Could not locate FCal-online-map.txt file" <<  endreq;
      return StatusCode::FAILURE;
    }
    const char * fcalmapfile= fcalmapname.c_str() ;
    std::ifstream infile (fcalmapfile) ;
    
    if(!infile) {
      log << MSG::ERROR
	  << " cannot open FCal-online-map.txt file "
	  << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::DEBUG
	  << " FCal-online-map.txt file opened "
	  << endreq;
    }
    
    // read the FCAL mapping file for all channels (from Gerald Oakham- oct 2007)
    
    // file structure: side (A/C), module (1-3), phi, eta (offline)
    //                 FT_slot, FEB_channel, Trig-group, Trig-eta, Trig-phi 
    std::string side;
    
    short int module;
    short int phi;
    short int eta;
    
    short int slot;
    short int chan;
    
    std::string TTlabel;
    short int l1_eta;
    std::string TTphi;
    
    int iline=0;
    
    while ( infile >> side >> module >> phi >> eta
	    >>  slot >> chan
	    >> TTlabel >> l1_eta >> TTphi) {
      iline++;
      LArTTCell_t t ;
      
      // fields for the offline cell id
      int pos_neg=2; // A side
      if(side=="C") {pos_neg=-2;}
      
      t.det = 2;
      t.pn  = pos_neg;
      t.sample = 0;
      t.region = module;
      t.eta = eta; 
      t.phi = phi;
      // check that the id makes sense:
      bool connected = m_fcalHelper->is_connected(t.pn,t.region,t.eta,t.phi); 
      if(!connected) {
	log << MSG::ERROR 
	    << "wrong dictionary ? cell not connected ! pn= "
	    << t.pn << "reg= " << t.region << "eta= " << t.eta << "phi= " << t.phi 
	    << endreq;
      }
      
      // fields for the offline TT channel id
      int l1_pn=1; // A side
      if(side=="C") {l1_pn=-1;}
      int l1_samp=1;
      if(module==1) {l1_samp=0;}
      int l1_phi = int(TTphi[0])-64;
      int l1_lay=0;
      if(module==3) {l1_lay=1;}
      
      t.tpn  = l1_pn;
      t.tsample = l1_samp;
      t.tregion = 3;
      t.teta = l1_eta -1;    // online numbering starts at 1 (!) contrary to offline numbering (0)
      t.tphi = l1_phi -1;    // online numbering starts at 1 (!) contrary to offline numbering (0)
      t.layer = l1_lay     ;
    
      d.push_back(t); 
  }
    infile.close() ;

    log << MSG::INFO
	<< " Read FCAL online-offline-TT map,nb of lines= "
	<< iline  << endreq;
  }

  // store map in StoreGate
  LArTTCellMap *ttMap = new LArTTCellMap();
  if ( ttMap == 0 ){
    log << MSG::ERROR 
	<< "Could not allocate a new LArTTCellMap" 
	<< endreq;
    return StatusCode::FAILURE;	  
  }

  // register the Map into the TES 
  StatusCode sc = m_detStore->record( ttMap ,  "LArTTCellMapAtlas") ;
  if( sc.isFailure() )
  {
    log << MSG::ERROR 
	<< "Could not record new LArTTCellMap in TES " 
	<< endreq;
    return StatusCode::FAILURE;	  
  }

  ttMap->set(d);
      

  sc = m_detStore->setConst( ttMap ) ;
  if( sc.isFailure() )
  {
    log << MSG::ERROR 
	<< "Could not lock ttMap in detStore " 
	<< endreq;
    return StatusCode::FAILURE;	  
  }


  return StatusCode::SUCCESS ;
}

// ====================================================================================
StatusCode initTTMap_Algo::testStruct(){
// ====================================================================================

// currently only runs with no old map as input
// (initial initialization with hard-coded map)

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "=> initTTMap_Algo::testStruct " << endreq;
  
  const LArTTCellMap* ttCellMap_c ;
  StatusCode sc = m_detStore->retrieve( ttCellMap_c ) ;
    
  if ( sc.isFailure() || !ttCellMap_c)  {
    log << MSG::ERROR << "testStruct: could not retrieve LArTTCellMap " 
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  LArTTCellMap*   ttCellMap = const_cast<LArTTCellMap*>(ttCellMap_c); 
  LArTTCell_P* ttCell_P = ttCellMap->getP(); 
  typedef std::vector<LArTTCell_P::LArTTCell_P_t> VTTCELL; 
  VTTCELL::const_iterator it   = ttCell_P->m_v.begin(); 
  VTTCELL::const_iterator it_e = ttCell_P->m_v.end(); 
  log<<MSG::DEBUG<<"  Dump of LArTTCellMap" <<endreq; 
  log<<MSG::DEBUG<<" Persistent LArTTCell_P version = "<<ttCell_P->m_version<<endreq;
  int lines=0;
  std::ofstream* dumpFcal=0;
  std::ofstream* dumpOther=0;
  std::string fcalFile="initDumpFCAL.txt";
  std::string otherFile="initDumpOther.txt";
  if(m_dumpMap) {
    dumpFcal=new std::ofstream(fcalFile.c_str());   
    if (dumpFcal==0) {
      std::cout << "Problem opening FCAL dump file" << std::endl;
      return 1;
    }
    dumpOther=new std::ofstream(otherFile.c_str());   
    if (dumpOther==0) {
      std::cout << "Problem opening other dump file" << std::endl;
      return 1;
    }
  }
  for (; it!=it_e;++it)       {
    const LArTTCell_P::LArTTCell_P_t& t = *it;    
    lines++;
    log<<MSG::VERBOSE
       <<" det="<<t.det
       <<" pn="<<t.pn 
       <<" sample="<<t.sample
       <<" region="<<t.region
       <<" eta="<<t.eta
       <<" phi="<<t.phi
       <<" trig_pn="<<t.tpn
       <<" trig_samp="<<t.tsample
       <<" trig_reg="<<t.tregion
       <<" trig_eta="<<t.teta
       <<" trig_phi="<<t.tphi
       <<" layer="<<t.layer
       <<endreq;

    if(m_dumpMap) {

      if((t.det)==2) {
	*dumpFcal
	  <<" det="<<t.det
	  <<" pn="<<t.pn 
	  <<" sample="<<t.sample
	  <<" region="<<t.region
	  <<" eta="<<t.eta
	  <<" phi="<<t.phi
	  <<" trig_pn="<<t.tpn
	  <<" trig_samp="<<t.tsample
	  <<" trig_reg="<<t.tregion
	  <<" trig_eta="<<t.teta
	  <<" trig_phi="<<t.tphi
	  <<" layer="<<t.layer
	  <<std::endl;
      } else {
	*dumpOther
	  <<" det="<<t.det
	  <<" pn="<<t.pn 
	  <<" sample="<<t.sample
	  <<" region="<<t.region
	  <<" eta="<<t.eta
	  <<" phi="<<t.phi
	  <<" trig_pn="<<t.tpn
	  <<" trig_samp="<<t.tsample
	  <<" trig_reg="<<t.tregion
	  <<" trig_eta="<<t.teta
	  <<" trig_phi="<<t.tphi
	  <<" layer="<<t.layer
	  <<std::endl;
      }
    }

  }

  if(m_dumpMap) {
    dumpFcal->close();
    delete dumpFcal;
    dumpOther->close();
    delete dumpOther;
  }

  log<<MSG::DEBUG<<" number of lines found = "<< lines<<endreq;

  return StatusCode::SUCCESS ;
  
}

/********************************************************/
StatusCode initTTMap_Algo::finalize(){
	
  MsgStream log( messageService(), name() );	
  log << MSG::INFO << " finalizing " << endreq;
  return StatusCode::SUCCESS ; 
  
}
