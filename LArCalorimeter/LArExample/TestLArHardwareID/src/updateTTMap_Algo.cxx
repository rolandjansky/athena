/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "TestLArHardwareID/updateTTMap_Algo.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloTriggerTool/LArTTCell.h"
#include "CaloTriggerTool/LArTTCell_P.h"
#include "CaloTriggerTool/LArTTCellMap.h"

/********************************************************/
updateTTMap_Algo::updateTTMap_Algo(const std::string &name , ISvcLocator* pSvcLocator) :
  Algorithm( name , pSvcLocator) ,
  m_detStore(0),
  m_dumpMap(false)
{
      
  declareProperty("dumpMap", m_dumpMap );

}

updateTTMap_Algo::~updateTTMap_Algo()
{
}

// ==============================================================
StatusCode updateTTMap_Algo::initialize(){
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

  
  return sc;
}


// ====================================================================================
StatusCode updateTTMap_Algo::execute(){
// ====================================================================================

  MsgStream log( messageService(), name() );
  log << MSG::INFO << "=> updateTTMap_Algo::Executing " << endreq;

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

  // set version to 1
  ttCell_P->m_version = 1 ;
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

    if((t.det)==0 && (t.pn)==1 && (t.tregion)==0 && (t.tphi)<4){

      it++;
      nLineF++;

    } else {

      it = ttCell_P->m_v.erase(it);
      nLineE++;
      
    }

  }
  
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
  

  return StatusCode::SUCCESS ;
}

/********************************************************/
StatusCode updateTTMap_Algo::finalize(){
	
  MsgStream log( messageService(), name() );	
  log << MSG::INFO << " finalizing " << endreq;
  return StatusCode::SUCCESS ; 
  
}
