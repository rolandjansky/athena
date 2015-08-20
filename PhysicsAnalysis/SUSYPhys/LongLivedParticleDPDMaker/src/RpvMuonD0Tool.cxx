/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpvMuonD0Tool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Mike Flowerdew (Michael.Flowerdew@cern.ch)
// Tool to write lepton d0 in a way accessible by derivation framwork tools
// Writes result to SG for later selection by string parser

#include "LongLivedParticleDPDMaker/RpvMuonD0Tool.h"
#include "xAODMuon/MuonContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::RpvMuonD0Tool::RpvMuonD0Tool( const std::string& t,
						   const std::string& n,
						   const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_collName("Muons"),
  m_sgPrefix("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("CollectionName", m_collName);
    declareProperty("SGPrefix", m_sgPrefix);
  }
 
// Destructor
DerivationFramework::RpvMuonD0Tool::~RpvMuonD0Tool() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::RpvMuonD0Tool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::RpvMuonD0Tool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::RpvMuonD0Tool::addBranches() const
{

     // Retrieve data
     const xAOD::MuonContainer* muons =  evtStore()->retrieve< const xAOD::MuonContainer >( m_collName );
     if( ! muons ) {
	ATH_MSG_ERROR("Couldn't retrieve muon container with key: " << m_collName);
	return StatusCode::FAILURE;
     }
	
     // Make a vector for the cut results
     std::vector<float>* d0vec = new std::vector<float>();
     std::vector<int>* isCombinedVec = new std::vector<int>();

     // Loop over muons, set decisions
     for (xAOD::MuonContainer::const_iterator muIt = muons->begin(); muIt!=muons->end(); ++muIt) {
       
       float d0 = 9e99;
       // Alternative: (*muIt)->trackParticle( xAOD::Muon::InnerDetectorTrackParticle ) ?
       if (*muIt && (*muIt)->primaryTrackParticle()) d0 = (*muIt)->primaryTrackParticle()->d0();
       else ATH_MSG_WARNING("Problem accessing muon track!");
       
       d0vec->push_back(d0);
       //// is it a combined muon?
       if ((*muIt)->muonType()==xAOD::Muon::Combined) isCombinedVec->push_back(1);
       else isCombinedVec->push_back(0);
       

     }     

     // Write decision to SG for access by downstream algs 
     std::string sgKey(m_sgPrefix+"D0");

     if (evtStore()->contains<std::vector<float> >(sgKey)) {
       ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << sgKey << " which already exists. Please use a different key");
       return StatusCode::FAILURE;
     }
     CHECK(evtStore()->record(d0vec, sgKey)); 

     sgKey = m_sgPrefix+"isCombined";

     if (evtStore()->contains<std::vector<int> >(sgKey)) {
       ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << sgKey << " which already exists. Please use a different key");
       return StatusCode::FAILURE;
     }
     CHECK(evtStore()->record(isCombinedVec, sgKey));       

     

     
     return StatusCode::SUCCESS;

}

