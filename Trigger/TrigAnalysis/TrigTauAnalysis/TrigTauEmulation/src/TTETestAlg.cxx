/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "TTETestAlg.h"

#include "xAODJet/JetContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTracking/TrackParticle.h"


TTETestAlg::TTETestAlg(const std::string& name, ISvcLocator* svcLoc)
  : AthAlgorithm(name, svcLoc),
    m_emul(this)

{

  declareProperty("Level1EmulationTool", m_emul);

}


StatusCode TTETestAlg::initialize()

{

  ATH_MSG_INFO("Initialize");
  ATH_CHECK(m_emul.retrieve());
  return StatusCode::SUCCESS;

}


StatusCode TTETestAlg::execute()

{

  const xAOD::EmTauRoIContainer *l1taus = 0;
  ATH_CHECK(evtStore()->retrieve(l1taus, "LVL1EmTauRoIs"));

  const xAOD::JetRoIContainer *l1jets = 0;
  ATH_CHECK(evtStore()->retrieve(l1jets, "LVL1JetRoIs"));
  
  const xAOD::MuonRoIContainer* l1muons = 0;
  ATH_CHECK(evtStore()->retrieve(l1muons, "LVL1MuonRoIs"));

  const xAOD::EnergySumRoI* l1xe = 0;
  ATH_CHECK(evtStore()->retrieve(l1xe, "LVL1EnergySumRoI"));
     
  ATH_CHECK(m_emul->calculate(l1taus, l1jets, l1muons, l1xe));

  m_emul->PrintCounters();
  return StatusCode::SUCCESS;

}
