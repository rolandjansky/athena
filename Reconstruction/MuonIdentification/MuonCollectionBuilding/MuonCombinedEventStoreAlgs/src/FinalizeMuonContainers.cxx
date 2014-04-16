/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FinalizeMuonContainers.cxx  -  (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "FinalizeMuonContainers.h"
#include "muonEvent/MuonContainer.h"

Rec::FinalizeMuonContainers::FinalizeMuonContainers
(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_muonContainerNames()
{
  declareProperty("MuonContainerNames",m_muonContainerNames,
		  "Vector of input muon containers, which will be set const by this alg.");
}

//================ Initialisation =================================================
StatusCode Rec::FinalizeMuonContainers::initialize()
{

  if (m_muonContainerNames.size() == 0) {
    ATH_MSG_WARNING ("Misconfigured Finalise-MuonContainer-Alg: nothing to do!"<<std::endl<<
		     "inputs " << m_muonContainerNames);
  }
  ATH_MSG_DEBUG ("Initialization done in "<<name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================
StatusCode Rec::FinalizeMuonContainers::finalize()
{
  ATH_MSG_DEBUG("Finalize done for "<<name());
  return StatusCode::SUCCESS;
}

//================ Per-event execute ============================================
StatusCode Rec::FinalizeMuonContainers::execute()
{
  ATH_MSG_DEBUG("starting muon collection finalisation");

  // === Loop over containers, lock them by setting them const
  for (unsigned int i=0; i < m_muonContainerNames.size(); ++i) {
    const std::string& containerName = m_muonContainerNames[i];
    Analysis::MuonContainer* openContainer;
    if ( evtStore()->contains<Analysis::MuonContainer>(containerName) &&
         evtStore()->retrieve(openContainer,containerName).isSuccess()) {
      evtStore()->setConst(openContainer).ignore();
      ATH_MSG_DEBUG("Container "<<containerName<<" closed against further mod.");
    }    
  }
  return StatusCode::SUCCESS;
}
