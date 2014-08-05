/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_TBScint.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"
#include <fstream>
#include <string>
#include "PathResolver/PathResolver.h"


CBNTAA_TBScint::CBNTAA_TBScint(const std::string & name, ISvcLocator * pSvcLocator) : CBNT_TBRecBase(name, pSvcLocator),m_scint_num(0),m_scint_en(0),m_scint_trackid(0),m_scint_pdg(0)  {
  m_eventStore = 0;
}

StatusCode CBNTAA_TBScint::CBNT_initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in initialize()"  << endreq;

  StatusCode sc = service("StoreGateSvc", m_eventStore);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve pointer to StoreGate Service" << endreq;
    return sc;
  }

  IToolSvc* toolSvc;
  sc=service("ToolSvc",toolSvc);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Unable to retrieve ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }


  // Add scint parameters
  addBranch("bm_scint_num", m_scint_num);
  addBranch("bm_scint_en", m_scint_en);
  addBranch("bm_scint_trackid", m_scint_trackid);
  addBranch("bm_scint_pdg", m_scint_pdg);

  return StatusCode::SUCCESS; 
}

StatusCode CBNTAA_TBScint::CBNT_execute() {
  /// Print an informatory message:
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;
  
  StatusCode sc;
  double edep;
  int scnum, pcode, trid;
  unsigned i;
  
  // Get scint. hits
  LArG4H6FrontHitCollection *frontcoll;
  LArG4H6FrontHitCollection *movecoll;
  sc = m_eventStore->retrieve(frontcoll,"Front::Hits");
  if (sc.isSuccess()){
     LArG4H6FrontHitConstIterator f_it = frontcoll->begin();
     LArG4H6FrontHitConstIterator f_end = frontcoll->end();
     for ( ; f_it!=f_end; ++f_it) {
       LArG4H6FrontHit* hit = (*f_it);
       scnum = hit->GetSC();
       if(scnum  <= 0) continue; // not a scintilator hit
       edep = hit->GetEdep();
       pcode = hit->GetPcode();
       trid = hit->GetTrackID();
       // Check if we have already such combination
       for(i=0; i<m_scint_num->size(); ++i){
          if( (scnum == (*m_scint_num)[i]) && (pcode == (*m_scint_pdg)[i]) &&
              (trid  == (*m_scint_trackid)[i]) ) { //add energy
             
            (*m_scint_en)[i] += edep;
            break;
          }  
       }
       if(i == m_scint_num->size()) { // new combination
          m_scint_num->push_back(scnum);
          m_scint_pdg->push_back(pcode);
          m_scint_trackid->push_back(trid);
          m_scint_en->push_back(edep);
       }
     }
  } else {
    log << MSG::WARNING << "Retrieval of Front Hits failed..." << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_eventStore->retrieve(movecoll,"Movable::Hits");
  if (sc.isSuccess()){
     LArG4H6FrontHitConstIterator m_it = movecoll->begin();
     LArG4H6FrontHitConstIterator m_end = movecoll->end();
     for ( ; m_it!=m_end; ++m_it) {
       LArG4H6FrontHit* hit = (*m_it);
       scnum = hit->GetSC();
       if(scnum  <= 0) continue; // not a scintilator hit
       edep = hit->GetEdep();
       pcode = hit->GetPcode();
       trid = hit->GetTrackID();
       // Check if we have already such combination
       for(i=0; i<m_scint_num->size(); ++i){
          if( (scnum == (*m_scint_num)[i]) && (pcode == (*m_scint_pdg)[i]) &&
              (trid  == (*m_scint_trackid)[i]) ) { //add energy
             
            (*m_scint_en)[i] += edep;
            break;
          }  
       }
       if(i == m_scint_num->size()) { // new combination
          m_scint_num->push_back(scnum);
          m_scint_pdg->push_back(pcode);
          m_scint_trackid->push_back(trid);
          m_scint_en->push_back(edep);
       }
     }
  } else {
    log << MSG::WARNING << "Retrieval of Movable Hits failed" << endreq;
  }
       
  return StatusCode::SUCCESS;
}

StatusCode CBNTAA_TBScint::CBNT_finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}


StatusCode CBNTAA_TBScint::CBNT_clear()
{
   if(m_scint_num) m_scint_num->clear();
   if(m_scint_en) m_scint_en->clear();
   if(m_scint_trackid) m_scint_trackid->clear();
   if(m_scint_pdg) m_scint_pdg->clear();
  return StatusCode::SUCCESS;
}
