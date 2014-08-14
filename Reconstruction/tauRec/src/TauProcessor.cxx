/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ListItem.h"

#include "xAODTau/TauJetContainer.h"

#include "tauRec/TauProcessor.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauProcessor::TauProcessor(const std::string &name,
    ISvcLocator * pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_tauContainerName("TauRecContainer"),
m_tauAuxContainerName("TauRecContainerAux."),
m_AODmode(false),
m_tools(this) //make tools private
{
    declareProperty("TauContainer", m_tauContainerName);
    declareProperty("TauAuxContainer", m_tauAuxContainerName);
    declareProperty("Tools", m_tools, "List of TauToolBase tools");
    declareProperty("runOnAOD", m_AODmode); //AODS are input file
    
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauProcessor::~TauProcessor() {
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauProcessor::initialize() {


    //ATH_MSG_INFO("FF::TauProcessor :: initialize()");

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
StatusCode TauProcessor::finalize() {
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauProcessor::execute() {

    StatusCode sc;

    TauCandidateData rTauData;

    const xAOD::TauJetContainer*     pContainer = 0;
    const xAOD::TauJetAuxContainer*     pAuxContainer = 0;

    //-------------------------------------------------------------------------
    // retrieve Tau Containers from StoreGate
    //-------------------------------------------------------------------------
    sc = evtStore()->retrieve(pContainer, m_tauContainerName);
    if (sc.isFailure()) {
      if (m_AODmode) {
        // don't exit Athena if there is no Tau Container in (D)AODs when running in AOD mode
        // just exit TauProcessor
        // reason: somebody might use slimmed (D)AODs, where not needed containers are not present
        ATH_MSG_WARNING("Failed to retrieve " << m_tauContainerName << "! Will exit TauProcessor now!!");
        return StatusCode::SUCCESS;
      }
      else {
        ATH_MSG_FATAL("Failed to retrieve " << m_tauContainerName);
        return StatusCode::FAILURE;
      }
    } 
    rTauData.xAODTauContainer = const_cast<xAOD::TauJetContainer*>(pContainer);  

    sc = evtStore()->retrieve(pAuxContainer, m_tauAuxContainerName);
    if (sc.isFailure()) {
      if (m_AODmode) {
        // don't exit Athena if there is no Tau AuxContainer in (D)AODs when running in AOD mode
        // just exit TauProcessor
        // reason: somebody might use slimmed (D)AODs, where not needed containers are not present
        ATH_MSG_WARNING("Failed to retrieve " << m_tauAuxContainerName << "! Will exit TauProcessor now!!");
        return StatusCode::SUCCESS;
      }
      else {
        ATH_MSG_FATAL("Failed to retrieve " << m_tauAuxContainerName);
        return StatusCode::FAILURE;
      }
    } 
    rTauData.tauAuxContainer = const_cast<xAOD::TauJetAuxContainer*>(pAuxContainer);  

    // set TauCandidate properties
    rTauData.xAODTau = 0;
    /*
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

    //loop over taus
    xAOD::TauJetContainer::const_iterator tau_it  = pContainer->begin();
    xAOD::TauJetContainer::const_iterator tau_end = pContainer->end();
    
    for(; tau_it != tau_end; ++tau_it) {
        
        //-----------------------------------------------------------------
        // set tau candidate data for easy handling
        //-----------------------------------------------------------------
        rTauData.xAODTau          = const_cast<xAOD::TauJet * >( *tau_it);
	rTauData.seed = ( *rTauData.xAODTau->jetLink() );

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
          
            ATH_MSG_VERBOSE("The tau candidate has been modified");

        } else if (!sc.isSuccess()) {
            //TODO:cleanup of EndTools not necessary??
            //keep this here for future use (in case more than one seeding algo exist)
            /*
            ToolHandleArray<TauToolBase> ::iterator p_itT1 = m_tools.begin();
            for (; p_itT1 != p_itT; ++p_itT1)
                (*p_itT1)->cleanup(&rTauData);
            (*p_itT1)->cleanup(&rTauData);
             */
            //delete rTauData.tau;
        } else  {
            //delete rTauData.tau;
            }
    }



    //-------------------------------------------------------------------------
    // Finalize tools for this event
    //-------------------------------------------------------------------------

    itT = m_tools.begin();
    itTE = m_tools.end();
    for (; itT != itTE; ++itT) {
        sc = (*itT)->eventFinalize(&rTauData);
        if (sc != StatusCode::SUCCESS)
            return StatusCode::FAILURE;
    }


    ///////////////////////////////////////////////////////
    // locking of containers is moved to separate tau tool

    return StatusCode::SUCCESS;
}
