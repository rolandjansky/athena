/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauBuilderTool.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "tauRecTools/TauEventData.h"

//________________________________________
TauBuilderTool::TauBuilderTool(const std::string& type) :
  asg::AsgTool(type),
  m_tauContainerName("TauJets"),
  m_tauAuxContainerName("TauJetsAux."),
  m_tauTrackContainerName("TauTracks"),
  m_tauTrackAuxContainerName("TauTracksAux."),
  m_seedContainerName(""),
  m_maxEta(2.5),
  m_minPt(10000),
  m_doCreateTauContainers(false),
  m_data()
{
  declareProperty("TauContainer", m_tauContainerName);
  declareProperty("TauAuxContainer", m_tauAuxContainerName);
  declareProperty("TauTrackContainer", m_tauTrackContainerName);
  declareProperty("TauTrackAuxContainer", m_tauTrackAuxContainerName);
  declareProperty("SeedContainer", m_seedContainerName);
  declareProperty("MaxEta", m_maxEta);
  declareProperty("MinPt", m_minPt);
  declareProperty("doCreateTauContainers", m_doCreateTauContainers);
  declareProperty("Tools", m_tools, "List of ITauToolBase tools");
}

//________________________________________
TauBuilderTool::~TauBuilderTool(){}

//________________________________________
StatusCode TauBuilderTool::initialize(){
  StatusCode sc;

  //ATH_MSG_INFO("FF::TauBuilder :: initialize()");

  //-------------------------------------------------------------------------
  // No tools allocated!
  //-------------------------------------------------------------------------
  if (m_tools.size() == 0) {
    ATH_MSG_ERROR("no tools given!");
    return StatusCode::FAILURE;
  }

  //-------------------------------------------------------------------------
  // Allocate tools
  //-------------------------------------------------------------------------
  ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
  ATH_MSG_INFO("List of tools in execution sequence:");
  ATH_MSG_INFO("------------------------------------");

  unsigned int tool_count = 0;

  for (; itT != itTE; ++itT) {
    sc = itT->retrieve();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Cannot find tool named <" << *itT << ">");
      return StatusCode::FAILURE;
    } else {
      ++tool_count;
      //ATH_MSG_INFO((*itT)->type() << " - " << (*itT)->name());
      ATH_MSG_INFO((*itT)->name());
      //If you want to utlize TauCandidate In Tools, 
      //decalre TauCandidate in your class, and pass its address
      //to function below
      (*itT)->setTauEventData(&m_data);
    }
  }
  ATH_MSG_INFO(" ");
  ATH_MSG_INFO("------------------------------------");

  if (tool_count == 0) {
    ATH_MSG_ERROR("could not allocate any tool!");
    return StatusCode::FAILURE;
  }

  ///////////////////////////////////////////////////////////////////////////

  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauBuilderTool::execute(){

  StatusCode sc;
  m_data.clear();

  xAOD::TauJetContainer * pContainer = 0;
  xAOD::TauJetAuxContainer* pAuxContainer = 0;
  xAOD::TauTrackContainer* pTracks = 0;
  xAOD::TauTrackAuxContainer* pAuxTracks = 0;


  if (m_doCreateTauContainers) {        
    //-------------------------------------------------------------------------
    // Create and Record containers
    //-------------------------------------------------------------------------

    pContainer = new xAOD::TauJetContainer();
    sc = evtStore()->record( pContainer, m_tauContainerName ) ;
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to record TauContainer in TDS");
      delete pContainer;
      return StatusCode::FAILURE;
    }

    pAuxContainer = new xAOD::TauJetAuxContainer();
    sc = evtStore()->record( pAuxContainer, m_tauAuxContainerName ) ;
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to record TauContainer in TDS");
      delete pContainer;
      return StatusCode::FAILURE;
    }
      
    pContainer->setStore( pAuxContainer );
    ATH_MSG_DEBUG( "Recorded xAOD tau jets with key: "
		   << m_tauAuxContainerName );


    pTracks = new xAOD::TauTrackContainer();
    ATH_CHECK( evtStore()->record( pTracks, m_tauTrackContainerName) );
    pAuxTracks = new xAOD::TauTrackAuxContainer();
    ATH_CHECK( evtStore()->record( pAuxTracks, m_tauTrackAuxContainerName));
    pTracks->setStore( pAuxTracks );

  } else {
    //-------------------------------------------------------------------------
    // retrieve Tau Containers from StoreGate
    //-------------------------------------------------------------------------
    sc = evtStore()->retrieve(pContainer, m_tauContainerName);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve " << m_tauContainerName);
      return StatusCode::FAILURE;
    }

    sc = evtStore()->retrieve(pAuxContainer, m_tauAuxContainerName);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Failed to retrieve " << m_tauAuxContainerName);
      return StatusCode::FAILURE;
    }

  }

  //not strictly necessary to set the containers
  //but probably safer for tools to access
  //the tau container via TauEventData than
  //IParticle::container() (griffith)
  m_data.xAODTauContainer = pContainer;
  m_data.tauAuxContainer = pAuxContainer;

  //-------------------------------------------------------------------------
  // Initialize tools for this event
  //-------------------------------------------------------------------------
  ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    sc = (*itT)->eventInitialize();
    if (sc != StatusCode::SUCCESS)
      return StatusCode::FAILURE;
  }

  ////////////////////////////////////////////////////////

    
  //---------------------------------------------------------------------
  // Retrieve seed Container from TDS, return `failure' if not
  // existing
  //---------------------------------------------------------------------
  const xAOD::JetContainer * pSeedContainer;
  sc = evtStore()->retrieve(pSeedContainer, m_seedContainerName);
  // XXX still need to fix this. Currently only handle jets
  // const DataHandle<INavigable4MomentumCollection> pSeedContainer;
  // sc = evtStore()->retrieve(pSeedContainer, m_seedContainerName);
  if (sc.isFailure() || !pSeedContainer) {
    ATH_MSG_FATAL("No seed container with key:" << m_seedContainerName);
    return StatusCode::FAILURE;
  }

  //probably not necessary, but in case a tool wants this in the future
  //probably safer to retrieve via TauEventData than 
  //*(pTau.jetLink())->container()
  m_data.seedContainer = pSeedContainer;

  //---------------------------------------------------------------------
  // Loop over seeds
  //---------------------------------------------------------------------
  xAOD::JetContainer::const_iterator itS = pSeedContainer->begin();
  xAOD::JetContainer::const_iterator itSE = pSeedContainer->end();
  // XXX still need to fix this. Currently only handle jets
  // INavigable4MomentumCollection::const_iterator itS = pSeedContainer->begin();
  // INavigable4MomentumCollection::const_iterator itSE = pSeedContainer->end();

  ATH_MSG_VERBOSE("Number of seeds in the container: " << pSeedContainer->size());

  for (; itS != itSE; ++itS) {  

    const xAOD::Jet *pSeed = (*itS);
    ATH_MSG_VERBOSE("Seeds eta:" << pSeed->eta() << ", pt:" << pSeed->pt());

    if (fabs(pSeed->eta()) > m_maxEta) {
      ATH_MSG_VERBOSE("--> Seed rejected, eta out of range!");
      continue;
    }

    if (fabs(pSeed->pt()) < m_minPt) {
      ATH_MSG_VERBOSE("--> Seed rejected, pt out of range!");
      continue;
    }

    //-----------------------------------------------------------------
    // Seed passed cuts --> create tau candidate
    //-----------------------------------------------------------------
    xAOD::TauJet* pTau = new xAOD::TauJet();
    pContainer->push_back( pTau );
    pTau->setJet(pSeedContainer, pSeed);

    //-----------------------------------------------------------------
    // Process the candidate
    //-----------------------------------------------------------------
    ToolHandleArray<ITauToolBase>::iterator itT = m_tools.begin();
    ToolHandleArray<ITauToolBase>::iterator itTE = m_tools.end();

    //-----------------------------------------------------------------
    // Loop stops when Failure indicated by one of the tools
    //-----------------------------------------------------------------
    for (; itT != itTE; ++itT) {
      ATH_MSG_VERBOSE("Invoking tool " << (*itT)->name());

      sc = (*itT)->execute(*pTau);

      if (sc.isFailure())
	break;
    }

    if (sc.isSuccess()) {
            
         
      ATH_MSG_VERBOSE("The tau candidate has been registered");

      //-----------------------------------------------------------------
      // Process the candidate using endTools
      //-----------------------------------------------------------------
      //keep this here for future use (in case more than one seeding algo exist)
      /*
	ToolHandleArray<ITauToolBase> ::iterator p_itET = m_endTools.begin();
	ToolHandleArray<ITauToolBase> ::iterator p_itETE = m_endTools.end();
	for (; p_itET != p_itETE; ++p_itET) {
	ATH_MSG_VERBOSE("Invoking endTool " << (*p_itET)->name());
	p_sc = (*p_itET)->execute(&m_data);
	if (p_sc.isFailure())
	break;
	}
      */
      //////////////////////////////////////////////////////

    } else if (!sc.isSuccess()) {
      //TODO:cleanup of EndTools not necessary??
      //keep this here for future use (in case more than one seeding algo exist)
      /*
	ToolHandleArray<ITauToolBase> ::iterator p_itT1 = m_tools.begin();
	for (; p_itT1 != p_itT; ++p_itT1)
	(*p_itT1)->cleanup(&m_data);
	(*p_itT1)->cleanup(&m_data);
      */
      
      //remove orphaned tracks before tau is deleted via pop_back
      xAOD::TauJet* bad_tau = pContainer->back();
      ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
      pTracks->erase(pTracks->end()-bad_tau->nAllTracks(), pTracks->end());

      //m_data.xAODTauContainer->pop_back();
      pContainer->pop_back();
    } else{
      
      //remove orphaned tracks before tau is deleted via pop_back
      xAOD::TauJet* bad_tau = pContainer->back();
      ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
      pTracks->erase(pTracks->end()-bad_tau->nAllTracks(), pTracks->end());
      
      //m_data.xAODTauContainer->pop_back();
      pContainer->pop_back();
    }
  }



  //-------------------------------------------------------------------------
  // Finalize tools for this event
  //-------------------------------------------------------------------------

  itT = m_tools.begin();
  itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    sc = (*itT)->eventFinalize();
    if (sc != StatusCode::SUCCESS)
      return StatusCode::FAILURE;
  }


  //keep this here for future use (in case more than one seeding algo exist)
  /*
    p_itET = m_endTools.begin();
    p_itETE = m_endTools.end();
    for (; p_itET != p_itETE; ++p_itET) {
    p_sc = (*p_itET)->eventFinalize(&m_data);
    if (p_sc != StatusCode::SUCCESS)
    return StatusCode::FAILURE;
    }
  */

  ///////////////////////////////////////////////////////

  // locking of containers is moved to separate tau tool

  return StatusCode::SUCCESS;
}

//________________________________________
StatusCode TauBuilderTool::finalize(){

  return StatusCode::SUCCESS;
}

