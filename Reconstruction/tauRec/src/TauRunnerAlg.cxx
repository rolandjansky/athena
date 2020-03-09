/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
m_tools(this) //make tools private
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

    //-------------------------------------------------------------------------
    // No tools allocated!
    //-------------------------------------------------------------------------
    if (m_tools.size() == 0) {
        ATH_MSG_ERROR("no tools given!");
        return StatusCode::FAILURE;
    }

    ATH_CHECK( m_tauInputContainer.initialize() );
    ATH_CHECK( m_pi0ClusterInputContainer.initialize() );

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
    }
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("------------------------------------");

    if (tool_count == 0) {
        ATH_MSG_ERROR("could not allocate any tool!");
        return StatusCode::FAILURE;
    }

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
    ATH_MSG_VERBOSE("All the invoded tools are finilized successfully.");
  } 

  return StatusCode::SUCCESS;

}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauRunnerAlg::execute() {
  
  // write neutral PFO container
  SG::WriteHandle<xAOD::PFOContainer> neutralPFOHandle( m_neutralPFOOutputContainer );
  ATH_CHECK(neutralPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
  xAOD::PFOContainer* neutralPFOContainer = neutralPFOHandle.ptr();

  // write hadronic cluster PFO container
  SG::WriteHandle<xAOD::PFOContainer> hadronicPFOHandle( m_hadronicPFOOutputContainer );
  ATH_CHECK(hadronicPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
  xAOD::PFOContainer* hadronicClusterPFOContainer = hadronicPFOHandle.ptr();

  // write pi0 calo clusters
  SG::WriteHandle<xAOD::CaloClusterContainer> pi0CaloClusHandle( m_pi0ClusterOutputContainer );
  ATH_CHECK(pi0CaloClusHandle.record(std::make_unique<xAOD::CaloClusterContainer>(), std::make_unique<xAOD::CaloClusterAuxContainer>()));
  xAOD::CaloClusterContainer* pi0CaloClusterContainer = pi0CaloClusHandle.ptr();

  // write secondary vertices
  SG::WriteHandle<xAOD::VertexContainer> vertOutHandle( m_vertexOutputContainer );
  ATH_CHECK(vertOutHandle.record(std::make_unique<xAOD::VertexContainer>(), std::make_unique<xAOD::VertexAuxContainer>()));
  xAOD::VertexContainer* pSecVtxContainer = vertOutHandle.ptr();

  // write charged PFO container
  SG::WriteHandle<xAOD::PFOContainer> chargedPFOHandle( m_chargedPFOOutputContainer );
  ATH_CHECK(chargedPFOHandle.record(std::make_unique<xAOD::PFOContainer>(), std::make_unique<xAOD::PFOAuxContainer>()));
  xAOD::PFOContainer* chargedPFOContainer = chargedPFOHandle.ptr();

  // write pi0 container
  SG::WriteHandle<xAOD::ParticleContainer> pi0Handle( m_pi0Container );
  ATH_CHECK(pi0Handle.record(std::make_unique<xAOD::ParticleContainer>(), std::make_unique<xAOD::ParticleAuxContainer>()));
  xAOD::ParticleContainer* pi0Container = pi0Handle.ptr();
  
  // Read the CaloClusterContainer created by the CaloClusterMaker
  SG::ReadHandle<xAOD::CaloClusterContainer> pi0ClusterInHandle( m_pi0ClusterInputContainer );
  if (!pi0ClusterInHandle.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << pi0ClusterInHandle.key());
    return StatusCode::FAILURE;
  }
  const xAOD::CaloClusterContainer * pPi0ClusterContainer = pi0ClusterInHandle.cptr();
  
  // Read in temporary tau jets
  SG::ReadHandle<xAOD::TauJetContainer> tauInputHandle(m_tauInputContainer);
  if (!tauInputHandle.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << tauInputHandle.key());
    return StatusCode::FAILURE;
  }
  const xAOD::TauJetContainer* pTauContainer = tauInputHandle.cptr();

  // Write the output tau jets, which is a deep copy of the input ones 
  SG::WriteHandle<xAOD::TauJetContainer> outputTauHandle(m_tauOutputContainer);
  ATH_CHECK(outputTauHandle.record(std::make_unique<xAOD::TauJetContainer>(), std::make_unique<xAOD::TauJetAuxContainer>()));    
  xAOD::TauJetContainer* newTauCon = outputTauHandle.ptr();

  for (const xAOD::TauJet* tau : *pTauContainer) {
    xAOD::TauJet* newTau = new xAOD::TauJet();
    newTauCon->push_back(newTau);
    *newTau = *tau;
  }
  
    // iterate over the copy
    for (xAOD::TauJet* pTau : *newTauCon) {
      //-----------------------------------------------------------------
      // Loop stops when Failure indicated by one of the tools
      //-----------------------------------------------------------------
      StatusCode sc;
    
      ToolHandleArray<ITauToolBase> ::iterator itT = m_tools.begin();
      ToolHandleArray<ITauToolBase> ::iterator itTE = m_tools.end();
      for (; itT != itTE; ++itT) {
	ATH_MSG_DEBUG("RunnerAlg Invoking tool " << (*itT)->name());
	if ( (*itT)->name().find("Pi0ClusterCreator") != std::string::npos){
          sc = (*itT)->executePi0ClusterCreator(*pTau, *neutralPFOContainer, *hadronicClusterPFOContainer, *pi0CaloClusterContainer, *pPi0ClusterContainer);
        }
	else if ( (*itT)->name().find("VertexVariables") != std::string::npos){
	  sc = (*itT)->executeVertexVariables(*pTau, *pSecVtxContainer);
	}
	else if ( (*itT)->name().find("Pi0ClusterScaler") != std::string::npos){
	  sc = (*itT)->executePi0ClusterScaler(*pTau, *neutralPFOContainer, *chargedPFOContainer);
	}
	else if ( (*itT)->name().find("Pi0ScoreCalculator") != std::string::npos){
	  sc = (*itT)->executePi0nPFO(*pTau, *neutralPFOContainer);
	}
	else if ( (*itT)->name().find("Pi0Selector") != std::string::npos){
	  sc = (*itT)->executePi0nPFO(*pTau, *neutralPFOContainer);
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
      if (sc.isSuccess()) {
        ATH_MSG_VERBOSE("The tau candidate has been modified successfully by all the invoked tools.");
      }
    } // end iterator over shallow copy

  ATH_MSG_VERBOSE("The tau candidate container has been modified");

  return StatusCode::SUCCESS;
}
