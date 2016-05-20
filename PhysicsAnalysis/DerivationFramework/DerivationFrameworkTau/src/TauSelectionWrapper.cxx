/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauSelectionWrapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Wrapper around the passSelection() method of xAOD egamma
// Writes result to SG for later selection by string parser

#include "DerivationFrameworkTau/TauSelectionWrapper.h"
#include "xAODTau/TauJetContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TauSelectionWrapper::TauSelectionWrapper( const std::string& t,
                                                 const std::string& n,
                                                 const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_isTauFlag(-1),
  m_collName("TauRecContainer"),
  m_sgKey("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("IsTauFlag",m_isTauFlag);
    declareProperty("CollectionName", m_collName);
    declareProperty("StoreGateEntryName", m_sgKey);
  }
 
// Destructor
DerivationFramework::TauSelectionWrapper::~TauSelectionWrapper() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::TauSelectionWrapper::initialize()
{
     if (m_isTauFlag==-1) {
        ATH_MSG_ERROR("No tau flag variables for the tau selection wrapper tool!");
        return StatusCode::FAILURE;
     }
     if (m_collName=="") {
        ATH_MSG_ERROR("No tau container provided for the tau selection wrapper tool!");
        return StatusCode::FAILURE;
     }
     if (m_sgKey=="") {
        ATH_MSG_ERROR("No StoreGate key provided for the output of the tau selection wrapper tool!");
        return StatusCode::FAILURE;
     }
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::TauSelectionWrapper::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::TauSelectionWrapper::addBranches() const
{

     // Retrieve data
     const xAOD::TauJetContainer* taus =  evtStore()->retrieve< const xAOD::TauJetContainer >( m_collName );
     if( !taus ) {
	ATH_MSG_ERROR("Couldn't retrieve tau container with key: " << m_collName);
	return StatusCode::FAILURE;
     }
	
     // Make vectors for the cut results
     std::vector<int>* isTauSelectionResult = new std::vector<int>;
     // Loop over taus, set decisions   
     xAOD::TauJetParameters::IsTauFlag isTauFlag = (xAOD::TauJetParameters::IsTauFlag)m_isTauFlag;
     xAOD::TauJetContainer::const_iterator tauBegin = taus->begin();
     xAOD::TauJetContainer::const_iterator tauEnd = taus->end();
     for (xAOD::TauJetContainer::const_iterator tauIt = taus->begin(); tauIt!=taus->end(); ++tauIt) {
	if ( (*tauIt)->isTau(isTauFlag) ) {isTauSelectionResult->push_back(1);}
	else {isTauSelectionResult->push_back(0);}
     }     

     // Write decision to SG for access by downstream algs 
     if (evtStore()->contains<std::vector<int> >(m_sgKey)) {
     	ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgKey << " which already exists. Please use a different key");
	delete isTauSelectionResult;
     	return StatusCode::FAILURE;
     }
     CHECK(evtStore()->record(isTauSelectionResult,m_sgKey));       
     return StatusCode::SUCCESS;

}

