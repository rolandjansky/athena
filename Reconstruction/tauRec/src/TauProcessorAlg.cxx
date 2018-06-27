/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ListItem.h"

#include "tauRec/TauProcessorAlg.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"


#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauProcessorAlg::TauProcessorAlg(const std::string &name,
    ISvcLocator * pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_tools(this), //make tools private
m_maxEta(2.5),
m_minPt(10000),
m_doCreateTauContainers(false),
m_data()
{
  declareProperty("Tools", m_tools);
  declareProperty("MaxEta", m_maxEta);
  declareProperty("MinPt", m_minPt);
  declareProperty("doCreateTauContainers", m_doCreateTauContainers);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauProcessorAlg::~TauProcessorAlg() {
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::initialize() {


    //ATH_MSG_INFO("FF::TauProcessorAlg :: initialize()");

    //-------------------------------------------------------------------------
    // No tools allocated!
    //-------------------------------------------------------------------------
    if (m_tools.size() == 0) {
        ATH_MSG_ERROR("no tools given!");
        return StatusCode::FAILURE;
    }

    StatusCode sc;


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
            ATH_MSG_INFO((*itT)->type() << " - " << (*itT)->name());
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

    ATH_CHECK( m_jetInputContainer.initialize() );
    ATH_CHECK( m_tauOutputContainer.initialize() );
    ATH_CHECK( m_tauTrackOutputContainer.initialize() );
    ATH_CHECK( m_tauShotClusOutputContainer.initialize() );

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::finalize() {

  StatusCode sc;

  //-----------------------------------------------------------------
  // Loop stops when Failure indicated by one of the tools
  //-----------------------------------------------------------------
  ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
  ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
  for (; itT != itTE; ++itT) {
    ATH_MSG_VERBOSE("Invoking tool " << (*itT)->name());
    sc = (*itT)->finalize();
    if (sc.isFailure())
      break;
  }

  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE("The tau candidate container has been modified");
  } else if (!sc.isSuccess()) {
  } else  {
  }


  return StatusCode::SUCCESS;

}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauProcessorAlg::execute() {
  
  StatusCode sc;

  
    // Declare containers  
    xAOD::TauJetContainer * pContainer = 0;
    xAOD::TauJetAuxContainer* pAuxContainer = 0;
    xAOD::TauTrackContainer* pTauTrackCont = 0;
    xAOD::TauTrackAuxContainer* pTauTrackAuxCont = 0;

    // Declare write handles
    SG::WriteHandle<xAOD::TauJetContainer> tauHandle( m_tauOutputContainer );
    SG::WriteHandle<xAOD::TauTrackContainer> tauTrackHandle( m_tauTrackOutputContainer );

    if (m_doCreateTauContainers) {
      //-------------------------------------------------------------------------                         
      // Create and Record containers
      //-------------------------------------------------------------------------                 
      pContainer = new xAOD::TauJetContainer();
      pAuxContainer = new xAOD::TauJetAuxContainer();
      pContainer->setStore( pAuxContainer );

      pTauTrackCont = new xAOD::TauTrackContainer();
      pTauTrackAuxCont = new xAOD::TauTrackAuxContainer();
      pTauTrackCont->setStore( pTauTrackAuxCont );


    } else {
      //-------------------------------------------------------------------------                                             
      // retrieve Tau Containers from StoreGate                                                                                     
      //-------------------------------------------------------------------------                                    
      // replace with read handles
      sc = evtStore()->retrieve(pContainer, "TauJets");
      if (sc.isFailure()) {
	ATH_MSG_FATAL("Failed to retrieve " << "TauJets");
	return StatusCode::FAILURE;
      }
    }

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

    //---------------------------------------------------------------------                                                    
    // Retrieve seed Container from TDS, return `failure if no                                        
    // existing                                                                                                                        
    //---------------------------------------------------------------------                                                       
    SG::ReadHandle<xAOD::JetContainer> jetHandle( m_jetInputContainer );
    if (!jetHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << jetHandle.key());
      return StatusCode::FAILURE;
    }
    const xAOD::JetContainer *pSeedContainer = 0;
    pSeedContainer = jetHandle.cptr();

    // The calo cluster containter must be registered to storegate here, in order to set links in shot finder tool
    // Will still allow changes to the container within this algorithm
    SG::WriteHandle<xAOD::CaloClusterContainer> tauShotClusHandle( m_tauShotClusOutputContainer );
    xAOD::CaloClusterContainer* tauShotClusContainer = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer* tauShotClusAuxStore = new xAOD::CaloClusterAuxContainer();
    tauShotClusContainer->setStore(tauShotClusAuxStore);
    ATH_MSG_DEBUG("  write: " << tauShotClusHandle.key() << " = " << "..." );
    ATH_CHECK(tauShotClusHandle.record(std::unique_ptr<xAOD::CaloClusterContainer>{tauShotClusContainer}, std::unique_ptr<xAOD::CaloClusterAuxContainer>{tauShotClusAuxStore}));

    //---------------------------------------------------------------------                                                        
    // Loop over seeds
    //---------------------------------------------------------------------                                                 
    xAOD::JetContainer::const_iterator itS = pSeedContainer->begin();
    xAOD::JetContainer::const_iterator itSE = pSeedContainer->end();

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

      // This sets one track and link. Need to have at least 1 track linked to retrieve track container
      setEmptyTauTrack(pTau, pTauTrackCont);
      
      //-----------------------------------------------------------------
      // Loop stops when Failure indicated by one of the tools
      //-----------------------------------------------------------------
      ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
      ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
      for (; itT != itTE; ++itT) {
	ATH_MSG_INFO("ProcessorAlg Invoking tool " << (*itT)->name());
	
	if ( (*itT)->name().find("ShotFinder") != std::string::npos){
	  sc = (*itT)->executeCaloClus(*pTau, *tauShotClusContainer);
	}
	else {
	  sc = (*itT)->execute(*pTau);
	}
	if (sc.isFailure())
	  break;
      }

      if (sc.isSuccess()) {
	
	ATH_MSG_VERBOSE("The tau candidate has been registered");
     
      } else if (!sc.isSuccess()) {
	//remove orphaned tracks before tau is deleted via pop_back
	xAOD::TauJet* bad_tau = pContainer->back();
	ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
	pTauTrackCont->erase(pTauTrackCont->end()-bad_tau->nAllTracks(), pTauTrackCont->end());

	//m_data.xAODTauContainer->pop_back();
	pContainer->pop_back();
      } else{

	//remove orphaned tracks before tau is deleted via pop_back
	xAOD::TauJet* bad_tau = pContainer->back();
	ATH_MSG_DEBUG("Deleting " << bad_tau->nAllTracks() << "Tracks associated with tau: ");
	pTauTrackCont->erase(pTauTrackCont->end()-bad_tau->nAllTracks(), pTauTrackCont->end());

	//m_data.xAODTauContainer->pop_back();
	pContainer->pop_back();
      }


    }// loop through seeds

    itT = m_tools.begin();
    itTE = m_tools.end();
    for (; itT != itTE; ++itT) {
      sc = (*itT)->eventFinalize();
      if (sc != StatusCode::SUCCESS)
	return StatusCode::FAILURE;
    }


  if (sc.isSuccess()) {
    ATH_MSG_VERBOSE("The tau candidate container has been modified");
  } else if (!sc.isSuccess()) {
  } else  {
  }
  
  // Write the completed tau and track containers
  ATH_MSG_DEBUG("  write: " << tauHandle.key() << " = " << "..." );
  ATH_CHECK(tauHandle.record(std::unique_ptr<xAOD::TauJetContainer>{pContainer}, std::unique_ptr<xAOD::TauJetAuxContainer>{pAuxContainer}));
  ATH_MSG_DEBUG("  write: " << tauTrackHandle.key() << " = " << "..." );  
  ATH_CHECK(tauTrackHandle.record(std::unique_ptr<xAOD::TauTrackContainer>{pTauTrackCont}, std::unique_ptr<xAOD::TauTrackAuxContainer>{pTauTrackAuxCont}));

  return StatusCode::SUCCESS;
}


void TauProcessorAlg::setEmptyTauTrack(xAOD::TauJet* &pTau, 
				       xAOD::TauTrackContainer* &tauTrackContainer)
{  
  // Make a new tau track, add to container
  xAOD::TauTrack* pTrack = new xAOD::TauTrack();
  tauTrackContainer->push_back(pTrack);
    
  // Create an element link for that track
  ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
  linkToTauTrack.toContainedElement(*tauTrackContainer, pTrack);
  pTau->addTauTrackLink(linkToTauTrack);
}

