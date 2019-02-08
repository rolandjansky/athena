/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include "TestLArHardwareID/initTTMap_Algo.h"

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
#include "CaloTriggerTool/CaloTriggerTowerService.h"
#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/LArTTCellMap.h"
#include "CxxUtils/make_unique.h"

/********************************************************/
initTTMap_Algo::initTTMap_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  AthAlgorithm( name , pSvcLocator) ,
  m_initFromFile("OFF"),
  m_dumpMap(false),
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
  ATH_MSG_INFO ( " initializing " );

  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service( "ToolSvc",toolSvc ) );
 
  ATH_CHECK( toolSvc->retrieveTool("CaloTriggerTowerService", m_triggertowerSvc) );
  ATH_MSG_DEBUG ( "initialize() successfully retrieved CaloTriggerTowerService" );

  const CaloIdManager*	caloIdMgr = nullptr;
  ATH_CHECK( detStore()->retrieve(caloIdMgr) );
  ATH_MSG_INFO ( "Successfully retrieved CaloIdManager from DetectorStore" );
  
  const CaloLVL1_ID* caloId = caloIdMgr->getLVL1_ID();
  if (!caloId) {
    ATH_MSG_ERROR ( "Could not access calolvl1 ID helper" );
      return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed calolvl1 ID helper" );
    m_lvl1Helper = caloId ;
  }
    
  const LArEM_ID* emId = caloIdMgr->getEM_ID();
  if (!emId) {
    ATH_MSG_ERROR ( "Could not access lar EM ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed lar EM ID helper" );
    m_emHelper=emId;
  }
    
  const LArHEC_ID* hecId = caloIdMgr->getHEC_ID();
  if (!hecId) {
    ATH_MSG_ERROR ( "Could not access lar HEC ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed lar HEC ID helper" );
    m_hecHelper=hecId;
  }
    
  const LArFCAL_ID* fcalId = caloIdMgr->getFCAL_ID();
  if (!fcalId) {
    ATH_MSG_ERROR ( "Could not access lar FCAL ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed lar FCAL ID helper" );
    m_fcalHelper=fcalId;
  }
  
  return StatusCode::SUCCESS;
}


// ====================================================================================
StatusCode initTTMap_Algo::execute(){
// ====================================================================================

  ATH_MSG_INFO ( "=> initTTMap_Algo::Executing " );

  ATH_CHECK( initMap() );
  ATH_CHECK( testStruct() );
  return StatusCode::SUCCESS;
}
// ====================================================================================
StatusCode initTTMap_Algo::initMap(){
// ====================================================================================

  ATH_MSG_INFO ( "=> initTTMap_Algo::initMap " );

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
  LArTTCell d;

  // ....... loop on Trigger Towers

  ATH_MSG_DEBUG ( " Starting loop on Trigger Towers " );
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
	ATH_MSG_VERBOSE("non existing extented TT " << m_lvl1Helper->show_to_string(layerId));
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
	    ATH_MSG_VERBOSE("EM cell " << m_lvl1Helper->show_to_string(cellId));
	    cell_det = 0;
	    cell_pn = m_emHelper->barrel_ec(cellId);
	    cell_samp = m_emHelper->sampling(cellId);
	    cell_reg = m_emHelper->region(cellId);
	    cell_eta = m_emHelper->eta(cellId);
	    cell_phi = m_emHelper->phi(cellId);
	  }
	  else if(m_emHelper->is_lar_hec(cellId)) {
	    ATH_MSG_VERBOSE("HEC cell " << m_lvl1Helper->show_to_string(cellId));
	    cell_det = 1;
	    cell_pn = m_hecHelper->pos_neg(cellId);
	    cell_samp = m_hecHelper->sampling(cellId);
	    cell_reg = m_hecHelper->region(cellId);
	    cell_eta = m_hecHelper->eta(cellId);
	    cell_phi = m_hecHelper->phi(cellId);
	  }
	  else if(m_emHelper->is_lar_fcal(cellId)) {
	    ATH_MSG_VERBOSE("FCAL cell " << m_lvl1Helper->show_to_string(cellId));
	    fcal=true;
	    cell_det = 2;
	    cell_samp = 0;
            // FCAL sample==region
            cell_pn = m_fcalHelper->pos_neg(cellId);
            cell_reg = m_fcalHelper->module(cellId);
            cell_eta = m_fcalHelper->eta(cellId);
            cell_phi = m_fcalHelper->phi(cellId);
	  } else {
	    ATH_MSG_ERROR( "Cell not in EM, nor HEC, nor FCAL: "
                           << m_emHelper->show_to_string(cellId) 
                           << " from Trigger Tower " 
                           << m_emHelper->show_to_string(layerId) );
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
  ATH_MSG_INFO( "number of cells found, skipped= " << nCell << " " << nSkip );
  ATH_MSG_INFO( "number of Non Existing Extented TT= " << nNE );

  if(m_initFromFile == "ON") {
    // the FCAL part is read from Gerald Oakham's file

    std::string fcalmapname=PathResolver::find_file ("FCal-online-map.txt", "DATAPATH");
    if (fcalmapname == "") {
      ATH_MSG_ERROR ( "Could not locate FCal-online-map.txt file" );
      return StatusCode::FAILURE;
    }
    const char * fcalmapfile= fcalmapname.c_str() ;
    std::ifstream infile (fcalmapfile) ;
    
    if(!infile) {
      ATH_MSG_ERROR( " cannot open FCal-online-map.txt file " );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG ( " FCal-online-map.txt file opened " );
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

    ATH_MSG_INFO ( " Read FCAL online-offline-TT map,nb of lines= " << iline  );
  }

  // store map in StoreGate
  LArTTCellMap *ttMap = new LArTTCellMap();
  if ( ttMap == 0 ){
    ATH_MSG_ERROR( "Could not allocate a new LArTTCellMap" );
    return StatusCode::FAILURE;	  
  }

  // register the Map into the TES 
  ATH_CHECK( detStore()->record( ttMap ,  "LArTTCellMapAtlas") );

  ttMap->set(d);
  ATH_CHECK( detStore()->setConst( ttMap ) );
  return StatusCode::SUCCESS ;
}

// ====================================================================================
StatusCode initTTMap_Algo::testStruct(){
// ====================================================================================

// currently only runs with no old map as input
// (initial initialization with hard-coded map)

  ATH_MSG_INFO ( "=> initTTMap_Algo::testStruct " );
  
  const LArTTCellMap* ttCellMap_c = nullptr;
  ATH_CHECK( detStore()->retrieve( ttCellMap_c ) );
  
  LArTTCellMap*   ttCellMap = const_cast<LArTTCellMap*>(ttCellMap_c); 
  LArTTCell_P* ttCell_P = ttCellMap->getP(); 
  typedef std::vector<LArTTCell_P::LArTTCell_P_t> VTTCELL; 
  VTTCELL::const_iterator it   = ttCell_P->m_v.begin(); 
  VTTCELL::const_iterator it_e = ttCell_P->m_v.end(); 
  ATH_MSG_DEBUG("  Dump of LArTTCellMap" );
  ATH_MSG_DEBUG(" Persistent LArTTCell_P version = "<<ttCell_P->m_version);
  int lines=0;
  std::unique_ptr<std::ofstream> dumpFcal;
  std::unique_ptr<std::ofstream> dumpOther;
  std::string fcalFile="initDumpFCAL.txt";
  std::string otherFile="initDumpOther.txt";
  if(m_dumpMap) {
    dumpFcal=CxxUtils::make_unique<std::ofstream>(fcalFile.c_str());   
    if (dumpFcal==0) {
      std::cout << "Problem opening FCAL dump file" << std::endl;
      return StatusCode::SUCCESS;
    }
    dumpOther=CxxUtils::make_unique<std::ofstream>(otherFile.c_str());   
    if (dumpOther==0) {
      std::cout << "Problem opening other dump file" << std::endl;
      return StatusCode::SUCCESS;
    }
  }
  for (; it!=it_e;++it)       {
    const LArTTCell_P::LArTTCell_P_t& t = *it;    
    lines++;
    ATH_MSG_VERBOSE
      (" det="<<t.det
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
       );

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
    dumpOther->close();
  }

  ATH_MSG_DEBUG(" number of lines found = "<< lines);
  return StatusCode::SUCCESS ;
}

/********************************************************/
StatusCode initTTMap_Algo::finalize(){
	
  ATH_MSG_INFO ( " finalizing " );
  return StatusCode::SUCCESS ; 
}
