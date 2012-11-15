/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include "TestLArHardwareID/reinitTTMap_Algo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"

#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloID.h"
#include "CaloIdentifier/LArID_Exception.h"

#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/LArTTCellMap.h"

/********************************************************/
reinitTTMap_Algo::reinitTTMap_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  Algorithm( name , pSvcLocator) ,
  m_dumpMap(false),
  m_testOnly(true),
  m_begRun(0),
  m_endRun(0),
  m_begLB(0),
  m_endLB(0),
  m_detStore(0),
  m_fcalHelper(0),
  m_regSvc(0),
  m_streamName("CondStream1"),
  m_streamer(0)
{

  // TTmap Offline Property:
  // ======================
      
  declareProperty("dumpMap", m_dumpMap );
  declareProperty("testOnly", m_testOnly );
  declareProperty("begRun", m_begRun );
  declareProperty("endRun", m_endRun );
  declareProperty("begLB", m_begLB );
  declareProperty("endLB", m_endLB );
      
}

reinitTTMap_Algo::~reinitTTMap_Algo()
{
}

// ==============================================================
StatusCode reinitTTMap_Algo::initialize(){
// ==============================================================
  MsgStream log( messageService(), name() );
  log << MSG::INFO << " initializing " << endreq;

  StatusCode sc = service( "DetectorStore", m_detStore );
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to locate DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Successfully located DetectorStore" << endreq;
  }	

  const CaloIdManager*	caloIdMgr;
  sc = m_detStore->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve CaloIdManager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "Successfully retrieved CaloIdManager from DetectorStore" << endreq;
  
        
    const LArFCAL_ID* fcalId = caloIdMgr->getFCAL_ID();
    if (!fcalId) {
      log << MSG::ERROR << "Could not access lar FCAL ID helper" << endreq;
      return StatusCode::FAILURE;
    } else {
      log << MSG::INFO << "Successfully accessed lar FCAL ID helper" << endreq;
      m_fcalHelper=fcalId;
    } 
  }	

  // get IOV registration service
  sc = service("IOVRegistrationSvc", m_regSvc);
  if (sc.isFailure()) {
    log << MSG::INFO << "Unable to find IOVRegistrationSvc " << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Found IOVRegistrationSvc "  << endreq;
  }

  // get the output stream tool for writing
  IToolSvc* toolSvc = 0;
  sc = service("ToolSvc", toolSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< " Tool Service not found "
	<< endreq;
    return StatusCode::FAILURE;
  }
  sc = toolSvc->retrieveTool("AthenaPoolOutputStreamTool", m_streamName, m_streamer) ;
  if (sc.isFailure()) {
    log << MSG::INFO
	<< "Unable to find AthenaOutputStreamTool"
	<< endreq;
    return StatusCode::FAILURE;
  }
    
  
  return sc;
}


// ====================================================================================
StatusCode reinitTTMap_Algo::execute(){
// ====================================================================================

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "=> reinitTTMap_Algo::Executing " << endreq;

  StatusCode sc = initMap();

  if(sc!= StatusCode::FAILURE) {
    sc = testStruct();
  } else {
    log << MSG::ERROR << "reinitTTMap_Algo:: failed init map " << endreq;
  }

  return sc ;
}
// ====================================================================================
StatusCode reinitTTMap_Algo::initMap(){
// ====================================================================================

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "=> reinitTTMap_Algo::initMap " << endreq;


  // first keep the EM and HEC parts identical


  const LArTTCellMap* ttCellMap_c ;
  StatusCode sc = m_detStore->retrieve( ttCellMap_c ) ;
    
  if ( sc.isFailure() || !ttCellMap_c)     {
    log << MSG::ERROR << "Could not retrieve LArTTCellMap " 
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  LArTTCellMap*   ttCellMap = const_cast<LArTTCellMap*>(ttCellMap_c); 
  
  //
  // first read the initial structure
  //
  LArTTCell_P* ttCell_P = ttCellMap->getP(); 
  typedef std::vector<LArTTCell_P::LArTTCell_P_t> VTTCELL; 
  VTTCELL::iterator it   = ttCell_P->m_v.begin(); 

  log<<MSG::DEBUG<<" Initial LArTTCell_P version = "<<ttCell_P->m_version<<endreq;

  // set version to 2
  ttCell_P->m_version = 2 ;
  log<<MSG::DEBUG<<" Final LArTTCell_P version = "<<ttCell_P->m_version<<endreq;

  unsigned int nLineI=0;
  unsigned int nLineF=0;
  unsigned int nLineE=0;

  while (it!=ttCell_P->m_v.end()) {
    LArTTCell_P::LArTTCell_P_t& t = *it;    
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
    nLineI++;

    if((t.det)==2){
      // erase FCAL
      it = ttCell_P->m_v.erase(it);
      nLineE++;

    } else {
      // keep everything else
      it++;
      nLineF++;
      
    }

  }

  LArTTCell d;

  
  VTTCELL::iterator it2   = ttCell_P->m_v.begin(); 
  VTTCELL::iterator it_e2 = ttCell_P->m_v.end(); 
  for (; it2!=it_e2;++it2)       {
    LArTTCell_P::LArTTCell_P_t& t = *it2;    
    log<<MSG::VERBOSE
       <<" kept: det="<<t.det
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

  }

  log<<MSG::DEBUG<<" nb of lines in initial struct= " << nLineI <<endreq; 
  log<<MSG::DEBUG<<" nb of lines in final struct= " << nLineF <<endreq; 
  log<<MSG::DEBUG<<" nb of lines erased= " << nLineE <<endreq; 





  // new FCAL file now

  // the FCAL part is read from MODIFIED Gerald Oakham's file

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

  ttCellMap->set(d);

  
  log << MSG::DEBUG << "Writing TT-cell map to POOL file" << endreq;
  sc = m_streamer->connectOutput();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not connect stream to output" << endreq;
    return( StatusCode::FAILURE);
  }

  int npairs = 1;
  int index = 0;
  IAthenaOutputStreamTool::TypeKeyPairs  typeKeys(npairs);
 
  IAthenaOutputStreamTool::TypeKeyPair   cellMap("LArTTCellMap", "");
  typeKeys[index] = cellMap;
  ++index;
  
  log << MSG::DEBUG <<"Stream out for pairs:" <<endreq;
  for (unsigned int i = 0; i < typeKeys.size(); ++i) {
    log << MSG::DEBUG << typeKeys[i].first << " " << typeKeys[i].second << " "
	<< endreq;
  }
  
  sc = m_streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    log <<MSG::ERROR <<"Could not stream out ttCellMap" <<endreq;
    return( StatusCode::FAILURE);
  }
  
  sc = m_streamer->commitOutput();
  if (sc.isFailure()) {
    log <<MSG::ERROR <<"Could not commit output stream" <<endreq;
    return( StatusCode::FAILURE);
  }

  
  if(m_testOnly) {
    log << MSG::DEBUG
	<< " End of reinit process "
	<< endreq;
  } else {
    // register to cool
    sc = m_regSvc->registerIOV("LArTTCellMap","LArTTCellMapAtlas","/LAR/Identifier/LArTTCellMapAtlas","-HadFcalFix",m_begRun,m_endRun, m_begLB, m_endLB);
    if (sc.isFailure()) {
      log <<MSG::ERROR <<"Could not register in IOV DB for CaloTTOnOffIdMap" <<endreq;
      return( StatusCode::FAILURE);
    }
    log << MSG::INFO << "Registered LArTTCellMap in folder /LAR/Identifier/LArTTCellMapAtlas with tag -HadFcalFix" << endreq;
  }

  return StatusCode::SUCCESS ;
}

// ====================================================================================
StatusCode reinitTTMap_Algo::testStruct(){
// ====================================================================================

// currently only runs with no old map as input
// (initial initialization with hard-coded map)

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "=> reinitTTMap_Algo::testStruct " << endreq;
  
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
StatusCode reinitTTMap_Algo::finalize(){
	
  MsgStream log( messageService(), name() );	
  log << MSG::INFO << " finalizing " << endreq;
  return StatusCode::SUCCESS ; 
  
}
