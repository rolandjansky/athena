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
#include "CxxUtils/make_unique.h"

/********************************************************/
reinitTTMap_Algo::reinitTTMap_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  AthAlgorithm( name , pSvcLocator) ,
  m_dumpMap(false),
  m_testOnly(true),
  m_begRun(0),
  m_endRun(0),
  m_begLB(0),
  m_endLB(0),
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
  ATH_MSG_INFO ( " initializing " );

  const CaloIdManager*	caloIdMgr = nullptr;
  ATH_CHECK( detStore()->retrieve(caloIdMgr) );
  ATH_MSG_INFO ( "Successfully retrieved CaloIdManager from DetectorStore" );
  
        
  const LArFCAL_ID* fcalId = caloIdMgr->getFCAL_ID();
  if (!fcalId) {
    ATH_MSG_ERROR ( "Could not access lar FCAL ID helper" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO ( "Successfully accessed lar FCAL ID helper" );
    m_fcalHelper=fcalId;
  } 

  // get IOV registration service
  ATH_CHECK( service("IOVRegistrationSvc", m_regSvc) );
  ATH_MSG_DEBUG ( "Found IOVRegistrationSvc "  );

  // get the output stream tool for writing
  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service("ToolSvc", toolSvc) );
  ATH_CHECK( toolSvc->retrieveTool("AthenaPoolOutputStreamTool", m_streamName, m_streamer) );
  
  return StatusCode::SUCCESS;
}


// ====================================================================================
StatusCode reinitTTMap_Algo::execute(){
// ====================================================================================

  ATH_MSG_INFO ( "=> reinitTTMap_Algo::Executing " );

  ATH_CHECK( initMap() );
  ATH_CHECK( testStruct() );

  return StatusCode::SUCCESS;
}
// ====================================================================================
StatusCode reinitTTMap_Algo::initMap(){
// ====================================================================================

  ATH_MSG_INFO ( "=> reinitTTMap_Algo::initMap " );


  // first keep the EM and HEC parts identical


  const LArTTCellMap* ttCellMap_c = nullptr;
  ATH_CHECK( detStore()->retrieve( ttCellMap_c ) );
  LArTTCellMap*   ttCellMap = const_cast<LArTTCellMap*>(ttCellMap_c); 
  
  //
  // first read the initial structure
  //
  LArTTCell_P* ttCell_P = ttCellMap->getP(); 
  typedef std::vector<LArTTCell_P::LArTTCell_P_t> VTTCELL; 
  VTTCELL::iterator it   = ttCell_P->m_v.begin(); 

  ATH_MSG_DEBUG(" Initial LArTTCell_P version = "<<ttCell_P->m_version);

  // set version to 2
  ttCell_P->m_version = 2 ;
  ATH_MSG_DEBUG(" Final LArTTCell_P version = "<<ttCell_P->m_version);

  unsigned int nLineI=0;
  unsigned int nLineF=0;
  unsigned int nLineE=0;

  while (it!=ttCell_P->m_v.end()) {
    LArTTCell_P::LArTTCell_P_t& t = *it;    
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
    ATH_MSG_VERBOSE
      (" kept: det="<<t.det
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

  }

  ATH_MSG_DEBUG(" nb of lines in initial struct= " << nLineI );
  ATH_MSG_DEBUG(" nb of lines in final struct= " << nLineF );
  ATH_MSG_DEBUG(" nb of lines erased= " << nLineE );

  // new FCAL file now

  // the FCAL part is read from MODIFIED Gerald Oakham's file

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
    ATH_MSG_DEBUG( " FCal-online-map.txt file opened " );
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
  
  ATH_MSG_INFO( " Read FCAL online-offline-TT map,nb of lines= "
                << iline  );

  ttCellMap->set(d);
  
  ATH_MSG_DEBUG ( "Writing TT-cell map to POOL file" );
  ATH_CHECK( m_streamer->connectOutput() );

  int npairs = 1;
  int index = 0;
  IAthenaOutputStreamTool::TypeKeyPairs  typeKeys(npairs);
 
  IAthenaOutputStreamTool::TypeKeyPair   cellMap("LArTTCellMap", "");
  typeKeys[index] = cellMap;
  ++index;
  
  ATH_MSG_DEBUG ("Stream out for pairs:" );
  for (unsigned int i = 0; i < typeKeys.size(); ++i) {
    ATH_MSG_DEBUG ( typeKeys[i].first << " " << typeKeys[i].second << " " );
  }
  
  ATH_CHECK( m_streamer->streamObjects(typeKeys) );
  ATH_CHECK( m_streamer->commitOutput() );

  if(m_testOnly) {
    ATH_MSG_DEBUG( " End of reinit process " );
  } else {
    // register to cool
    ATH_CHECK( m_regSvc->registerIOV("LArTTCellMap","LArTTCellMapAtlas","/LAR/Identifier/LArTTCellMapAtlas","-HadFcalFix",m_begRun,m_endRun, m_begLB, m_endLB) );
    ATH_MSG_INFO ( "Registered LArTTCellMap in folder /LAR/Identifier/LArTTCellMapAtlas with tag -HadFcalFix" );
  }

  return StatusCode::SUCCESS ;
}

// ====================================================================================
StatusCode reinitTTMap_Algo::testStruct(){
// ====================================================================================

// currently only runs with no old map as input
// (initial initialization with hard-coded map)

  ATH_MSG_INFO ( "=> reinitTTMap_Algo::testStruct " );
  
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
      return 1;
    }
    dumpOther=CxxUtils::make_unique<std::ofstream>(otherFile.c_str());   
    if (dumpOther==0) {
      std::cout << "Problem opening other dump file" << std::endl;
      return 1;
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
StatusCode reinitTTMap_Algo::finalize(){
	
  ATH_MSG_INFO ( " finalizing " );
  return StatusCode::SUCCESS ; 
}
