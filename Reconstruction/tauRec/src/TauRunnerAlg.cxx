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

    ATH_CHECK( m_tauInputContainer.initialize() );

    ATH_CHECK( m_tauOutputContainer.initialize() );
    ATH_CHECK( m_neutralPFOOutputContainer.initialize() );
    ATH_CHECK( m_pi0ClusterOutputContainer.initialize() );
    ATH_CHECK( m_hadronicPFOOutputContainer.initialize() );
    ATH_CHECK( m_vertexOutputContainer.initialize() );
    ATH_CHECK( m_chargedPFOOutputContainer.initialize() );
    ATH_CHECK( m_pi0Container.initialize() );

    //-------------------------------------------------------------------------
    // Allocate tools
    //-------------------------------------------------------------------------
    ATH_CHECK( m_tools.retrieve() );

    ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
    ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();

    ATH_MSG_INFO("List of tools in execution sequence:");
    ATH_MSG_INFO("------------------------------------");
    unsigned int tool_count = 0;
    for (; itT != itTE; ++itT) {
      ++tool_count;
      ATH_MSG_INFO((*itT)->type() << " - " << (*itT)->name());
      (*itT)->setTauEventData(&m_data);
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

    // write pi0 calo clusters
    xAOD::CaloClusterContainer* pi0CaloClusterContainer = new xAOD::CaloClusterContainer();
    xAOD::CaloClusterAuxContainer* pi0CaloClusterAuxContainer = new xAOD::CaloClusterAuxContainer();
    pi0CaloClusterContainer->setStore(pi0CaloClusterAuxContainer);
    SG::WriteHandle<xAOD::CaloClusterContainer> pi0CaloClusHandle( m_pi0ClusterOutputContainer );
    ATH_MSG_DEBUG("  write: " << pi0CaloClusHandle.key() << " = " << "..." );
    ATH_CHECK(pi0CaloClusHandle.record(std::unique_ptr<xAOD::CaloClusterContainer>{pi0CaloClusterContainer}, std::unique_ptr<xAOD::CaloClusterAuxContainer>{pi0CaloClusterAuxContainer}));

    // write secondary vertices
    xAOD::VertexContainer* pSecVtxContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer* pSecVtxAuxContainer = new xAOD::VertexAuxContainer();
    pSecVtxContainer->setStore( pSecVtxAuxContainer );
    SG::WriteHandle<xAOD::VertexContainer> vertOutHandle( m_vertexOutputContainer );
    ATH_MSG_DEBUG("  write: " << vertOutHandle.key() << " = " << "..." );
    ATH_CHECK(vertOutHandle.record(std::unique_ptr<xAOD::VertexContainer>{pSecVtxContainer}, std::unique_ptr<xAOD::VertexAuxContainer>{pSecVtxAuxContainer}));

    // write charged PFO container
    xAOD::PFOContainer* chargedPFOContainer = new xAOD::PFOContainer();
    xAOD::PFOAuxContainer* chargedPFOAuxStore = new xAOD::PFOAuxContainer();
    chargedPFOContainer->setStore(chargedPFOAuxStore);
    SG::WriteHandle<xAOD::PFOContainer> chargedPFOHandle( m_chargedPFOOutputContainer );
    ATH_MSG_DEBUG("  write: " << chargedPFOHandle.key() << " = " << "..." );
    ATH_CHECK(chargedPFOHandle.record(std::unique_ptr<xAOD::PFOContainer>{chargedPFOContainer}, std::unique_ptr<xAOD::PFOAuxContainer>{chargedPFOAuxStore}));

    // write pi0 container
    xAOD::ParticleContainer* pi0Container = new xAOD::ParticleContainer();
    xAOD::ParticleAuxContainer* pi0AuxStore = new xAOD::ParticleAuxContainer();
    pi0Container->setStore(pi0AuxStore);
    SG::WriteHandle<xAOD::ParticleContainer> pi0Handle( m_pi0Container );
    ATH_MSG_DEBUG("  write: " << pi0Handle.key() << " = " << "..." );
    ATH_CHECK(pi0Handle.record(std::unique_ptr<xAOD::ParticleContainer>{pi0Container}, std::unique_ptr<xAOD::ParticleAuxContainer>{pi0AuxStore}));
  
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

    // Read in temporary tau jets
    SG::ReadHandle<xAOD::TauJetContainer> tauInputHandle(m_tauInputContainer);
    if (!tauInputHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << tauInputHandle.key());
      return StatusCode::FAILURE;
    }
    pTauContainer = tauInputHandle.cptr();

    // Make new container which is deep copy of that
    xAOD::TauJetContainer* newTauCon = 0;
    xAOD::TauJetAuxContainer* newTauAuxCon = 0;
    xAOD::TauJet* tau(0);
    // See function in header file
    ATH_CHECK(deepCopy(newTauCon, newTauAuxCon, tau, pTauContainer));
    // Write final taujets container
    SG::WriteHandle<xAOD::TauJetContainer> outputTauHandle(m_tauOutputContainer);
    ATH_CHECK( outputTauHandle.record(std::unique_ptr<xAOD::TauJetContainer>(newTauCon), 
				      std::unique_ptr<xAOD::TauJetAuxContainer>(newTauAuxCon)) );    
    
    // iterate over the copy
    xAOD::TauJetContainer::iterator itTau = newTauCon->begin();
    xAOD::TauJetContainer::iterator itTauE = newTauCon->end();
    for (; itTau != itTauE; ++itTau) {

      xAOD::TauJet* pTau = (*itTau);
      //-----------------------------------------------------------------
      // Loop stops when Failure indicated by one of the tools
      //-----------------------------------------------------------------
      ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
      ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
      for (; itT != itTE; ++itT) {
	ATH_MSG_DEBUG("RunnerAlg Invoking tool " << (*itT)->name());
	if ( (*itT)->name().find("Pi0ClusterCreator") != std::string::npos){
          sc = (*itT)->executePi0ClusterCreator(*pTau, *neutralPFOContainer, *hadronicClusterPFOContainer, *pi0CaloClusterContainer);
        }
	else if ( (*itT)->name().find("VertexVariables") != std::string::npos){
	  sc = (*itT)->executeVertexVariables(*pTau, *pSecVtxContainer);
	}
	else if ( (*itT)->name().find("Pi0ClusterScaler") != std::string::npos){
	  sc = (*itT)->executePi0ClusterScaler(*pTau, *neutralPFOContainer, *chargedPFOContainer);
	}
	else if ( (*itT)->name().find("PanTau") != std::string::npos){
	  sc = (*itT)->executePanTau(*pTau, *pi0Container);
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
  
  return StatusCode::SUCCESS;
}
