/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEDM_Test/TopEDM_WriteTest.h"

#include "TopEvent/TTbarJJ.h"
#include "TopEvent/TTbarLJ.h"
#include "TopEvent/TTbarLL.h"
#include "TopEvent/TTbarJJTop.h"
#include "TopEvent/TTbarLJTop.h"
#include "TopEvent/TTbarLJMet.h"
#include "TopEvent/TTbarLJDiTops.h"
#include "TopEvent/TTbarLLJets.h"
#include "TopEvent/TTbarLLJetsMet.h"

#include "TopEvent/TTbarJJContainer.h"
#include "TopEvent/TTbarLJContainer.h"
#include "TopEvent/TTbarLLContainer.h"
#include "TopEvent/TTbarJJTopContainer.h"
#include "TopEvent/TTbarLJMetContainer.h"
#include "TopEvent/TTbarLJTopContainer.h"
#include "TopEvent/TTbarLJDiTopsContainer.h"
#include "TopEvent/TTbarLLJetsContainer.h"
#include "TopEvent/TTbarLLJetsMetContainer.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"

#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"

#include "MissingETEvent/MissingET.h"

#include "PhysicsElementLinks/JetLinks.h"


//================================================================
TopEDM_WriteTest::TopEDM_WriteTest(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("jetCollectionName", m_jetCollectionName="Cone4H1TowerJets");
  declareProperty("electronCollectionName", m_electronCollectionName="ElectronAODCollection");
  declareProperty("muonCollectionName", m_muonCollectionName="StacoMuonCollection");
  declareProperty("missingETName", m_missingETName="MET_RefFinal");
}

//================================================================
StatusCode TopEDM_WriteTest::initialize()
{
  ATH_MSG_INFO("TopEDM_WriteTest::initialize()");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TopEDM_WriteTest::finalize() 
{
  ATH_MSG_INFO("TopEDM_WriteTest::finalize()");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TopEDM_WriteTest::execute() {
  using namespace TopRec;

  ATH_MSG_INFO("TopEDM_WriteTest::execute() begin");

  const JetCollection* jets(0);
  if(! (sgSvc()->retrieve(jets, m_jetCollectionName).isSuccess()) ) {
    ATH_MSG_FATAL("Could not get jet container");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_INFO("Got jet container: "<<jets->size()<<" jets");
  }

  //----------------------------------------------------------------
  const ElectronContainer* electrons(0);
  if(! (sgSvc()->retrieve(electrons, m_electronCollectionName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get electron container");
  }
  else {
    ATH_MSG_INFO("Got electron container: "<<electrons->size()<<" electrons");
  }

  //----------------------------------------------------------------
  const Analysis::MuonContainer* muons(0);
  if(! (sgSvc()->retrieve(muons, m_muonCollectionName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get muon container");
  }
  else {
    ATH_MSG_INFO("Got muon container: "<<muons->size()<<" muons");
  }

  const MissingET* met(0);
  if(! (sgSvc()->retrieve(met, m_missingETName).isSuccess()) ) {
    ATH_MSG_WARNING("Could not get MissingET object");
  }
  else {
    ATH_MSG_INFO("Got MissingET object, et=" << met->et());
  }

  //================================================================
  TTbarJJContainer *jjContainer = new TTbarJJContainer();
  TTbarJJTopContainer *jjTopContainer = new TTbarJJTopContainer();
  TTbarEJContainer *ejContainer = new TTbarEJContainer();
  TTbarEJTopContainer *ejtopContainer = new TTbarEJTopContainer();
  TTbarEJDiTopsContainer *ejditopsContainer = new TTbarEJDiTopsContainer();
  TTbarEEContainer *eeContainer = new TTbarEEContainer();
  TTbarEEJetsContainer *eejContainer = new TTbarEEJetsContainer();

  TTbarMuJContainer *mujContainer = new TTbarMuJContainer();

  TTbarEJMetContainer *ejMetContainer = new TTbarEJMetContainer();
  TTbarEEJetsMetContainer * eejMetContainer = new TTbarEEJetsMetContainer();

  if(jets && jets->size() > 0) {
    ATH_MSG_INFO("Adding a JJ candidate to the container");
    
    JetLinks jetLinks;

    IndexSet jn;
    for(unsigned i=0; i<jets->size(); ++i) {
      jetLinks.push_back(ElementLink<JetCollection>(*jets, i));
      jn.insert(i);
    }

    TTbarJJ tjj(jets, jn);
    jjContainer->push_back(tjj);
    
    ATH_MSG_INFO("Adding a JJTop candidate to the container");
    IndexSet hadronicTopIndexes;
    for(unsigned i=0; i<jets->size()/4; ++i) { // may be empty... so what?
      hadronicTopIndexes.insert(i);
    }

    jjTopContainer->push_back(TTbarJJTop(tjj, hadronicTopIndexes));
    
    //----------------------------------------------------------------
    if(electrons && !electrons->empty()) {
      ATH_MSG_INFO("Adding a EJ candidate to the container");
      ElementLink<ElectronContainer> el(*electrons, 0/*electron index*/);

      TTbarEJ tej(el, jets, jn);
      ejContainer->push_back(tej);

      ATH_MSG_INFO("Adding a EJMet candidate to the container");
      DataLink<MissingET> metLink(m_missingETName);
      ejMetContainer->push_back(TTbarEJMet(el, metLink, jetLinks));

      ATH_MSG_INFO("Adding a EJTop candidate to the container");
      ejtopContainer->push_back(TTbarEJTop(tej, hadronicTopIndexes));

      if (jets->size() >= 4) {
	ATH_MSG_INFO("Adding a EJDiTops candidate to the container");
	P4PxPyPzE neutrino(10,0,0,10);
	ejditopsContainer->push_back(TTbarEJDiTops(tej, 0, neutrino, 1));
	
      }

      // dielectron candidates
      if(electrons->size()>1) {
	ATH_MSG_INFO("Adding a EEJets candidate to the container");
	ElementLink<ElectronContainer> el1(*electrons, 0/*electron index*/);
	ElementLink<ElectronContainer> el2(*electrons, 1/*electron index*/);
	eejContainer->push_back(TTbarEEJets( el1, el2, jets, jn));

	ATH_MSG_INFO("Adding EEJestMet candidate to the container");
	eejMetContainer->push_back(TTbarEEJetsMet(el1, el2, metLink, jetLinks));
      }
      else {
	ATH_MSG_INFO("Only one electron, will use empty EEJets container");
      }

    }
    else {
      ATH_MSG_INFO("No electrons, will use empty EJ container");
    }

    //----------------------------------------------------------------
    if(muons && !muons->empty()) {
      ATH_MSG_INFO("Adding a MuJ candidate to the container");
      ElementLink<Analysis::MuonContainer> mu(*muons, 0/*the index*/);
      mujContainer->push_back(TTbarMuJ( mu, jets, jn));
    }
    else {
      ATH_MSG_INFO("No muons, will use empty MuJ container");
    }

    //----------------------------------------------------------------
  }
  else {
    ATH_MSG_INFO("No jets, will use empty JJ and EJ containers");
  }

  //----------------------------------------------------------------
  if(electrons->size()>1) {
    ATH_MSG_INFO("Adding a EE candidate to the container");
    ElementLink<ElectronContainer> el1(*electrons, 0/*electron index*/);
    ElementLink<ElectronContainer> el2(*electrons, 1/*electron index*/);
    eeContainer->push_back(TTbarEE( el1, el2));
  }
  else {
    ATH_MSG_INFO("Only one electron, will use empty EE container");
  }

  //----------------------------------------------------------------
  if(!sgSvc()->record(jjContainer, "TTbarJJCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarJJCandidates");
  }
  if(!sgSvc()->record(jjTopContainer, "TTbarJJTopCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarJJTopCandidates");
  }
  if(!sgSvc()->record(ejContainer, "TTbarEJCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarEJCandidates");
  }
  if(!sgSvc()->record(ejtopContainer, "TTbarEJTopCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarEJTopCandidates");
  }
  if(!sgSvc()->record(ejditopsContainer, "TTbarEJDiTopsCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarEJDiTopsCandidates");
  }
  if(!sgSvc()->record(eeContainer, "TTbarEECandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarEECandidates");
  }
  if(!sgSvc()->record(eejContainer, "TTbarEEJetsCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarEEJetsCandidates");
  }

  if(!sgSvc()->record(mujContainer, "TTbarMuJCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarMuJCandidates");
  }

  if(!sgSvc()->record(eejMetContainer, "TTbarEEJetsMetCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarEEJetsMetCandidates");
  }
  if(!sgSvc()->record(ejMetContainer, "TTbarEJMetCandidates").isSuccess()) {
    ATH_MSG_WARNING("Problem recording TTbarEJMetCandidates");
  }

  ATH_MSG_INFO("TopEDM_WriteTest::execute() end");
  return StatusCode::SUCCESS;
}

