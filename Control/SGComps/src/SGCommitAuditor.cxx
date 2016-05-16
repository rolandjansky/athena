/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGCommitAuditor.h"

SGCommitAuditor::SGCommitAuditor(const std::string& name,
				 ISvcLocator* pSvcLocator) 
  :Auditor(name,pSvcLocator),
   p_sg("StoreGateSvc", name)
{

}

SGCommitAuditor::~SGCommitAuditor() {
}


StatusCode
SGCommitAuditor::initialize() {
  if (p_sg.retrieve().isFailure()) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::ERROR << "Could not retrieve \"" << p_sg.typeAndName() 
	<< "\"" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode
SGCommitAuditor::finalize() {
  return StatusCode::SUCCESS;
}



void
SGCommitAuditor::afterExecute(INamedInterface* /*alg*/, const StatusCode&) {
  p_sg->commitNewDataObjects();
}

