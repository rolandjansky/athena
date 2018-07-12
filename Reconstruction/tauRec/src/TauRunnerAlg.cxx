/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ListItem.h"

#include "tauRec/TauRunnerAlg.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"


#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "xAODCore/ShallowCopy.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauRunnerAlg::TauRunnerAlg(const std::string &name,
    ISvcLocator * pSvcLocator) :
AthAlgorithm(name, pSvcLocator),
m_tools(this), //make tools private
m_data()
{
  declareProperty("Tools", m_tools);
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauRunnerAlg::~TauRunnerAlg() {
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TauRunnerAlg::initialize() {


    //ATH_MSG_INFO("FF::TauRunnerAlg :: initialize()");

    //-------------------------------------------------------------------------
    // No tools allocated!
    //-------------------------------------------------------------------------
    if (m_tools.size() == 0) {
        ATH_MSG_ERROR("no tools given!");
        return StatusCode::FAILURE;
    }

    ATH_CHECK( m_neutralPFOOutputContainer.initialize() );
    ATH_CHECK( m_pi0ClusterOutputContainer.initialize() );
    ATH_CHECK( m_hadronicPFOOutputContainer.initialize() );

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

    ATH_CHECK( m_tauInputContainer.initialize() );
    ATH_CHECK( m_tauOutputContainer.initialize() );

    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauRunnerAlg::finalize() {

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
StatusCode TauRunnerAlg::execute() {
  
  StatusCode sc;



    // write neutral PFO container
    xAOD::PFOContainer* neutralPFOContainer = new xAOD::PFOContainer();
    xAOD::PFOAuxContainer* neutralPFOAuxStore = new xAOD::PFOAuxContainer();
    neutralPFOContainer->setStore(neutralPFOAuxStore);
    SG::WriteHandle<xAOD::PFOContainer> neutralPFOHandle( m_neutralPFOOutputContainer );
    ATH_MSG_DEBUG("  write: " << neutralPFOHandle.key() << " = " << "..." );
    ATH_CHECK(neutralPFOHandle.record(std::unique_ptr<xAOD::PFOContainer>{neutralPFOContainer}, std::unique_ptr<xAOD::PFOAuxContainer>{neutralPFOAuxStore}));

    // write hadronic cluster PFO container
    xAOD::PFOContainer* hadronicClusterPFOContainer = new xAOD::PFOContainer();
    xAOD::PFOAuxContainer* hadronicClusterPFOAuxStore = new xAOD::PFOAuxContainer();
    hadronicClusterPFOContainer->setStore(hadronicClusterPFOAuxStore);
    SG::WriteHandle<xAOD::PFOContainer> hadronicPFOHandle( m_hadronicPFOOutputContainer );
    ATH_MSG_DEBUG("  write: " << hadronicPFOHandle.key() << " = " << "..." );
    ATH_CHECK(hadronicPFOHandle.record(std::unique_ptr<xAOD::PFOContainer>{hadronicClusterPFOContainer}, std::unique_ptr<xAOD::PFOAuxContainer>{hadronicClusterPFOAuxStore}));

    // pi0 calo clusters
    xAOD::CaloClusterContainer* pi0CaloClusterContainer = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer* pi0CaloClusterAuxContainer = new xAOD::CaloClusterAuxContainer();
    pi0CaloClusterContainer->setStore(pi0CaloClusterAuxContainer);
    SG::WriteHandle<xAOD::CaloClusterContainer> pi0CaloClusHandle( m_pi0ClusterOutputContainer );
    ATH_MSG_DEBUG("  write: " << pi0CaloClusHandle.key() << " = " << "..." );
    ATH_CHECK(pi0CaloClusHandle.record(std::unique_ptr<xAOD::CaloClusterContainer>{pi0CaloClusterContainer}, std::unique_ptr<xAOD::CaloClusterAuxContainer>{pi0CaloClusterAuxContainer}));
  
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

    // Declare container
    const xAOD::TauJetContainer * pTauContainer = 0;

    // Read in tau jets
    SG::ReadHandle<xAOD::TauJetContainer> tauInputHandle(m_tauInputContainer);
    if (!tauInputHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << tauInputHandle.key());
      return StatusCode::FAILURE;
    }
    pTauContainer = tauInputHandle.cptr();

    // create shallow copy, write that
    std::pair< xAOD::TauJetContainer*, xAOD::ShallowAuxContainer* > taus_shallowCopy = xAOD::shallowCopyContainer( *pTauContainer );

    SG::WriteHandle<xAOD::TauJetContainer> outputTauHandle(m_tauOutputContainer);
    ATH_CHECK( outputTauHandle.record(std::unique_ptr<xAOD::TauJetContainer>(taus_shallowCopy.first), 
				      std::unique_ptr<xAOD::ShallowAuxContainer>(taus_shallowCopy.second)) );    
    
    // iterate over the shallow copy
    xAOD::TauJetContainer::iterator itTau = (taus_shallowCopy.first)->begin();
    xAOD::TauJetContainer::iterator itTauE = (taus_shallowCopy.first)->end();
    for (; itTau != itTauE; ++itTau) {

      xAOD::TauJet* pTau = (*itTau);
      //-----------------------------------------------------------------
      // Loop stops when Failure indicated by one of the tools
      //-----------------------------------------------------------------
      ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
      ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
      for (; itT != itTE; ++itT) {
	ATH_MSG_INFO("RunnerAlg Invoking tool " << (*itT)->name());
	if ( (*itT)->name().find("Pi0ClusterCreator") != std::string::npos){
          sc = (*itT)->executePi0ClusterCreator(*pTau, *neutralPFOContainer, *hadronicClusterPFOContainer, *pi0CaloClusterContainer);
        }
	else {
	  sc = (*itT)->execute(*pTau);
	}
	if (sc.isFailure())
	  break;
      }

    } // end iterator over shallow copy

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
  
  ATH_MSG_INFO("Done Runner alg");
  return StatusCode::SUCCESS;
}
