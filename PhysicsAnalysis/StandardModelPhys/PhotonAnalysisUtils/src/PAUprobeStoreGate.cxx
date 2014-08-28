/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/DataHandle.h"

#include "PhotonAnalysisUtils/PAUprobeStoreGate.h"
#include "PhotonAnalysisUtils/PAUegammaContainer.h"


PAUprobeStoreGate::PAUprobeStoreGate(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_storegatesvc(0)
{ }

StatusCode PAUprobeStoreGate::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode PAUprobeStoreGate::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode PAUprobeStoreGate::execute() {
  StatusCode sc = service( "StoreGateSvc", m_storegatesvc ) ;
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not access StoreGateSvc");
    return sc ;
  }
  const PAU::egammaContainer* gammaContainer;
  sc = m_storegatesvc->retrieve(gammaContainer,"PAUgammaCollection");
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not access PAUgammaCollection");
    return sc ;
  }
  ATH_MSG_INFO("PAUgammaCollection accessed successfully");
  gammaContainer->print();
  return StatusCode::SUCCESS;
}
