/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEDM_Test/TopEDM_ReadTest.h"

#include "TopEvent/TTbarJJ.h"
#include "TopEvent/TTbarLJ.h"
#include "TopEvent/TTbarLL.h"
#include "TopEvent/TTbarJJTop.h"
#include "TopEvent/TTbarLJTop.h"
#include "TopEvent/TTbarLJDiTops.h"
#include "TopEvent/TTbarLLJets.h"
#include "TopEvent/TTbarLJMet.h"
#include "TopEvent/TTbarLLJetsMet.h"

#include "TopEvent/TTbarJJContainer.h"
#include "TopEvent/TTbarLJContainer.h"
#include "TopEvent/TTbarLLContainer.h"
#include "TopEvent/TTbarJJTopContainer.h"
#include "TopEvent/TTbarLJTopContainer.h"
#include "TopEvent/TTbarLJDiTopsContainer.h"
#include "TopEvent/TTbarLLJetsContainer.h"
#include "TopEvent/TTbarLJMetContainer.h"
#include "TopEvent/TTbarLLJetsMetContainer.h"

#include "StoreGate/StoreGateSvc.h"

#include "JetEvent/JetCollection.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "MissingETEvent/MissingET.h"

//================================================================
TopEDM_ReadTest::TopEDM_ReadTest(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("ttbarJJContainerName", m_ttbarJJContainerName="TTbarJJCandidates");
  declareProperty("ttbarJJTopContainerName", m_ttbarJJTopContainerName="TTbarJJTopCandidates");
  declareProperty("ttbarEJContainerName", m_ttbarEJContainerName="TTbarEJCandidates");
  declareProperty("ttbarEJTopContainerName", m_ttbarEJTopContainerName="TTbarEJTopCandidates");
  declareProperty("ttbarEJDiTopsContainerName", m_ttbarEJDiTopsContainerName="TTbarEJDiTopsCandidates");
  declareProperty("ttbarEEContainerName", m_ttbarEEContainerName="TTbarEECandidates");
  declareProperty("ttbarEEJetsContainerName", m_ttbarEEJetsContainerName="TTbarEEJetsCandidates");
  declareProperty("ttbarEJMetContainerName", m_ttbarEJMetContainerName="TTbarEJMetCandidates");
  declareProperty("ttbarEEJetsMetContainerName", m_ttbarEEJetsMetContainerName="TTbarEEJetsMetCandidates");
}

//================================================================
StatusCode TopEDM_ReadTest::initialize()
{
  ATH_MSG_INFO("TopEDM_ReadTest::initialize()");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TopEDM_ReadTest::finalize() 
{
  ATH_MSG_INFO("TopEDM_ReadTest::finalize()");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TopEDM_ReadTest::execute() {
  using namespace TopRec;

  ATH_MSG_INFO("TopEDM_ReadTest::execute() begin");

  //----------------------------------------------------------------
  const TTbarJJContainer *jjContainer(0);
  if(! (sgSvc()->retrieve(jjContainer, m_ttbarJJContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarJJContainer with key "<<m_ttbarJJContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarJJContainer: "<<jjContainer->size()<<" candidates");
  }

  //----------------------------------------------------------------
  const TTbarJJTopContainer *jjTopContainer(0);
  if(! (sgSvc()->retrieve(jjTopContainer, m_ttbarJJTopContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarJJTopContainer with key "<<m_ttbarJJTopContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarJJTopContainer: "<<jjTopContainer->size()<<" candidates");
  }

  //----------------------------------------------------------------
  const TTbarEJContainer *ejContainer(0);
  if(! (sgSvc()->retrieve(ejContainer, m_ttbarEJContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarEJContainer with key "<<m_ttbarEJContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarEJContainer: "<<ejContainer->size()<<" candidates");
  }

  //----------------------------------------------------------------
  const TTbarEJMetContainer *ejMetContainer(0);
  if(! (sgSvc()->retrieve(ejMetContainer, m_ttbarEJMetContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarEJMetContainer with key "<<m_ttbarEJMetContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarEJMetContainer: "<<ejMetContainer->size()<<" candidates");
    for(TTbarEJMetContainer::const_iterator it = ejMetContainer->begin();
	it!=ejMetContainer->end(); ++it) {
      const MissingET* met = it->metLink();
      ATH_MSG_INFO("met->et()="<<met->et());
    }
  }

  //----------------------------------------------------------------
  const TTbarEJTopContainer *ejtopContainer(0);
  if(! (sgSvc()->retrieve(ejtopContainer, m_ttbarEJTopContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarEJTopContainer with key "<<m_ttbarEJTopContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarEJTopContainer: "<<ejtopContainer->size()<<" candidates");
  }

  //----------------------------------------------------------------
  const TTbarEJDiTopsContainer *ejditopsContainer(0);
  if(! (sgSvc()->retrieve(ejditopsContainer, m_ttbarEJDiTopsContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarEJDiTopsContainer with key "<<m_ttbarEJDiTopsContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarEJDiTopsContainer: "<<ejditopsContainer->size()<<" candidates");
  }

  //----------------------------------------------------------------
  const TTbarEEContainer *eeContainer(0);
  if(! (sgSvc()->retrieve(eeContainer, m_ttbarEEContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarEEContainer with key "<<m_ttbarEEContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarEEContainer: "<<eeContainer->size()<<" candidates");
  }

  //----------------------------------------------------------------
  const TTbarEEJetsContainer *eejContainer(0);
  if(! (sgSvc()->retrieve(eejContainer, m_ttbarEEJetsContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarEEJetsContainer with key "<<m_ttbarEEJetsContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarEEJetsContainer: "<<eejContainer->size()<<" candidates");
  }

  //----------------------------------------------------------------
  const TTbarEEJetsMetContainer *eejMetContainer(0);
  if(! (sgSvc()->retrieve(eejMetContainer, m_ttbarEEJetsMetContainerName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get TTbarEEJetsMetContainer with key "<<m_ttbarEEJetsMetContainerName);
  }
  else {
    ATH_MSG_INFO("Got TTbarEEJetsMetContainer: "<<eejMetContainer->size()<<" candidates");
    for(TTbarEEJetsMetContainer::const_iterator it = eejMetContainer->begin();
	it!=eejMetContainer->end(); ++it) {
      const MissingET* met = it->metLink();
      ATH_MSG_INFO("met->et()="<<met->et());
    }
  }

  //----------------------------------------------------------------
  ATH_MSG_INFO("TopEDM_ReadTest::execute() end");
  return StatusCode::SUCCESS;
}
