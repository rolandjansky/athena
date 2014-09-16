/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/System.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "MuonIdHelpersAlgs/CscHashId.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <algorithm>
#include <cmath>

using namespace std;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

CscHashId::CscHashId(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_EvtStore(NULL), m_cscId(NULL), m_muon_mgr(NULL)
   {

}

CscHashId::~CscHashId()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode CscHashId::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " in initialize()" << endreq;
            
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);

  if (sc.isSuccess()) {
    sc = detStore->retrieve( m_muon_mgr );
    if (sc.isFailure()) {
      log << MSG::ERROR << " Cannot retrieve MuonGeoModelMgr " << endreq;
      return sc;
    } else {
      m_cscId = m_muon_mgr->cscIdHelper();
    }
  } else {
    log << MSG::ERROR << " MuonDetDescrMgr not found in DetectorStore " << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode CscHashId::execute() {

  static  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  sc = cscHash();
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to execute CscHashId " << endreq;
  }
  return sc;
}

StatusCode CscHashId::finalize() {
 
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in finalize()" << endreq;

   return StatusCode::SUCCESS;

}

StatusCode CscHashId::cscHash() {
  StatusCode sc;
  static  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute(): testing CSC IdHelper" << endreq;

  // Loop over csc detector elements and add in the hash ids
  std::vector<Identifier> modules = m_cscId->idVector();

  typedef std::vector<Identifier>::const_iterator csc_id;

  csc_id firstId = modules.begin();
  csc_id lastId  = modules.end();

  IdContext moduleContext  =  m_cscId->module_context();
  IdContext channelContext =  m_cscId->channel_context();
  IdentifierHash moduleHashId;
  IdentifierHash channelHashId;

  for(; firstId != lastId; ++firstId) {
    Identifier id = *firstId;
    if (m_cscId->validElement(id)) {
      if(!m_cscId->get_hash(id, moduleHashId, &moduleContext)) {
	std::cout << "The CSC Chamber hash id is " << moduleHashId
	    << " for " << m_cscId->show_to_string(id,&moduleContext) << std::endl;
        std::cout << "*****The strips in this Chamber ********************" << std::endl;
        std::vector<Identifier> vect;
        m_cscId->idChannels(id, vect);
        for (unsigned int i=0; i<vect.size(); ++i) {
           if(!m_cscId->get_hash(vect[i], channelHashId, &channelContext)) {
	      std::cout << "The CSC strip hash id is " << channelHashId
	          << " for " << m_cscId->show_to_string(vect[i],&channelContext) << std::endl;
           }
        } 
      }
      else {
	log << MSG::ERROR << "Unable to set csc hash id for det elem " 
	    << "context begin_index = " << moduleContext.begin_index()
	    << " context end_index  = " << moduleContext.end_index()
	    << " the idenitifier is "
	    << endreq;
	id.show();
      }
    } else {
      log << MSG::ERROR << "Invalid CSC id " << endreq;
      id.show();
    }
  }  
  
 return StatusCode::SUCCESS;

}

