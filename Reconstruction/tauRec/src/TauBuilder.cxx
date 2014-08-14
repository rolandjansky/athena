/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauBuilder.cxx
// package:     Reconstruction/tauRec
// authors:     Srini Rajagopalan, Michael Heldmann, Lukasz Janyst,
//              Anna Kaczmarska
// date:        2007-02-13
// modification: 
//              15/04/2008 - (AK) fixing memory leak bug #35463
//              19/05/2011 - (KG) added EndAthTools to transition between AlgTools and AthAlgTools
//              22/05/2011 - (FF) changed to inherit from AthAlgorithm
//              23/05/2011 - (FF) removed EndAthTools -> transition finished
//              01/12/2011 - (FF) remove track seeded containers
//-----------------------------------------------------------------------------

#include "tauRec/TauBuilder.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"

#include "GaudiKernel/ListItem.h"
#include "tauRec/TauCandidateData.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TauBuilder::TauBuilder(const std::string &name,
    ISvcLocator * pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_tauContainerName("TauRecContainer"),
m_tauAuxContainerName("TauRecContainerAux."),
m_seedContainerName(""),
m_maxEta(2.5),
m_minPt(10000),
m_doCreateTauContainers(false),
m_tools(this) //make tools private
{
    declareProperty("TauContainer", m_tauContainerName);
    declareProperty("TauAuxContainer", m_tauAuxContainerName);
    declareProperty("SeedContainer", m_seedContainerName);
    declareProperty("MaxEta", m_maxEta);
    declareProperty("MinPt", m_minPt);
    declareProperty("doCreateTauContainers", m_doCreateTauContainers);
    declareProperty("Tools", m_tools, "List of TauToolBase tools");
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------

TauBuilder::~TauBuilder() {
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------

StatusCode TauBuilder::initialize() {
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
    ToolHandleArray<TauToolBase> ::iterator itT = m_tools.begin();
    ToolHandleArray<TauToolBase> ::iterator itTE = m_tools.end();
    ATH_MSG_INFO("List of tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");

    unsigned int tool_count = 0;

    for (; itT != itTE; ++itT) {
        sc = itT->retrieve();
        if (sc.isFailure()) {
            ATH_MSG_WARNING("Cannot find tool named <" << *itT << ">");
        } else {
            ++tool_count;
            ATH_MSG_INFO((*itT)->type() << " - " << (*itT)->name());
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

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------

StatusCode TauBuilder::finalize() {
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------

StatusCode TauBuilder::execute() {
  //ATH_MSG_INFO("FF::TauBuilder :: execute()");
  StatusCode sc;

  TauCandidateData rTauData;

  xAOD::TauJetContainer * pContainer = 0;
  xAOD::TauJetAuxContainer* pAuxContainer = 0;



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

      // pExtraDetailsContainer = new Analysis::TauDetailsContainer();
        // sc = evtStore()->record(pExtraDetailsContainer, m_tauExtraDetailsContainerName);
        // if (sc.isFailure()) {
        //     ATH_MSG_ERROR("Unable to record TauDetailsContainer in TDS");
        //     delete pExtraDetailsContainer;
        //     return StatusCode::FAILURE;
        // }

	// pPi0CandidateDetailsContainer = new Analysis::TauDetailsContainer();
	// sc = evtStore()->record(pPi0CandidateDetailsContainer, "TauPi0CandidateDetailsContainer");
        // if (sc.isFailure()) {
        //     ATH_MSG_ERROR("Unable to record TauPi0CandidateDetailsContainer in TDS");
        //     delete pPi0CandidateDetailsContainer;
        //     return StatusCode::FAILURE;
        // }


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

	// sc = evtStore()->retrieve(pExtraDetailsContainer, m_tauExtraDetailsContainerName);
        // if (sc.isFailure()) {
        //     ATH_MSG_FATAL("Failed to retrieve " << m_tauExtraDetailsContainerName);
        //     return StatusCode::FAILURE;
        // }

        // sc = evtStore()->retrieve(pPi0CandidateDetailsContainer, "TauPi0CandidateDetailsContainer");
        // if (sc.isFailure()) {
        //     ATH_MSG_FATAL("Failed to retrieve " << "TauPi0CandidateDetailsContainer");
        //     return StatusCode::FAILURE;
        // }
    }

    // set TauCandidate properties
    rTauData.xAODTau = 0;
    rTauData.xAODTauContainer = pContainer;
    rTauData.tauAuxContainer = pAuxContainer;
    

    //XXX leave this here for now until xAOD migration is completed
    /*
    rTauData.tauContainer = 0;
    rTauData.detailsContainer = 0;
    rTauData.extraDetailsContainer = 0;
    rTauData.pi0DetailsContainer = 0;
    rTauData.tau = 0;
    rTauData.details = 0;
    rTauData.extraDetails = 0;
    rTauData.pi0Details = 0;
    */
    rTauData.seed = 0;
    rTauData.seedContainer = 0;

    //-------------------------------------------------------------------------
    // Initialize tools for this event
    //-------------------------------------------------------------------------
    ToolHandleArray<TauToolBase> ::iterator itT = m_tools.begin();
    ToolHandleArray<TauToolBase> ::iterator itTE = m_tools.end();
    for (; itT != itTE; ++itT) {
        sc = (*itT)->eventInitialize(&rTauData);
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

    //---------------------------------------------------------------------
    // Loop over seeds
    //---------------------------------------------------------------------
    xAOD::JetContainer::const_iterator itS = pSeedContainer->begin();
    xAOD::JetContainer::const_iterator itSE = pSeedContainer->end();
    // XXX still need to fix this. Currently only handle jets
    // INavigable4MomentumCollection::const_iterator itS = pSeedContainer->begin();
    // INavigable4MomentumCollection::const_iterator itSE = pSeedContainer->end();

    ATH_MSG_VERBOSE("Number of seeds in the container: " << pSeedContainer->size());

    rTauData.seedContainer = pSeedContainer;

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

        rTauData.seed = pSeed;

        //-----------------------------------------------------------------
        // Seed passed cuts --> create tau candidate
        //-----------------------------------------------------------------
        rTauData.xAODTau          = new xAOD::TauJet();
        rTauData.xAODTauContainer->push_back( rTauData.xAODTau );

        //-----------------------------------------------------------------
        // Process the candidate
        //-----------------------------------------------------------------
        ToolHandleArray<TauToolBase>::iterator itT = m_tools.begin();
        ToolHandleArray<TauToolBase>::iterator itTE = m_tools.end();

        //-----------------------------------------------------------------
        // Loop stops when Failure indicated by one of the tools
        //-----------------------------------------------------------------
        for (; itT != itTE; ++itT) {
            ATH_MSG_VERBOSE("Invoking tool " << (*itT)->name());

            sc = (*itT)->execute(&rTauData);

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
            ToolHandleArray<TauToolBase> ::iterator p_itET = m_endTools.begin();
            ToolHandleArray<TauToolBase> ::iterator p_itETE = m_endTools.end();
            for (; p_itET != p_itETE; ++p_itET) {
                ATH_MSG_VERBOSE("Invoking endTool " << (*p_itET)->name());
                p_sc = (*p_itET)->execute(&rTauData);
                if (p_sc.isFailure())
                    break;
            }
             */
            //////////////////////////////////////////////////////

        } else if (!sc.isSuccess()) {
            //TODO:cleanup of EndTools not necessary??
            //keep this here for future use (in case more than one seeding algo exist)
            /*
            ToolHandleArray<TauToolBase> ::iterator p_itT1 = m_tools.begin();
            for (; p_itT1 != p_itT; ++p_itT1)
                (*p_itT1)->cleanup(&rTauData);
            (*p_itT1)->cleanup(&rTauData);
             */
	  rTauData.xAODTauContainer->pop_back();
        } else
	  rTauData.xAODTauContainer->pop_back();
    }



    //-------------------------------------------------------------------------
    // Finalize tools for this event
    //-------------------------------------------------------------------------
    //TODO: line below necessary?
    rTauData.xAODTau = 0;

    itT = m_tools.begin();
    itTE = m_tools.end();
    for (; itT != itTE; ++itT) {
        sc = (*itT)->eventFinalize(&rTauData);
        if (sc != StatusCode::SUCCESS)
            return StatusCode::FAILURE;
    }

    //keep this here for future use (in case more than one seeding algo exist)
    /*
    p_itET = m_endTools.begin();
    p_itETE = m_endTools.end();
    for (; p_itET != p_itETE; ++p_itET) {
        p_sc = (*p_itET)->eventFinalize(&rTauData);
        if (p_sc != StatusCode::SUCCESS)
            return StatusCode::FAILURE;
    }
     */

    ///////////////////////////////////////////////////////

    // locking of containers is moved to separate tau tool

    return StatusCode::SUCCESS;
}
