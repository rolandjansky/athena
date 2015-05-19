/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpvMuonMSTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Authors: Mike Flowerdew (Michael.Flowerdew@cern.ch), Dominik Krauss (dominik.krauss@cern.ch)
// Augments a muon with a flag whether it has MS track
// Writes result to SG for later selection by string parser

#include "LongLivedParticleDPDMaker/RpvMuonMSTool.h"
#include "xAODMuon/MuonContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::RpvMuonMSTool::RpvMuonMSTool( const std::string& t,
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
DerivationFramework::RpvMuonMSTool::~RpvMuonMSTool() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::RpvMuonMSTool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::RpvMuonMSTool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::RpvMuonMSTool::addBranches() const
{
     // Retrieve data
     const xAOD::MuonContainer* muons = evtStore()->retrieve<const xAOD::MuonContainer>(m_collName);
     if(!muons) {
	ATH_MSG_ERROR("Couldn't retrieve muon container with key: " << m_collName);
	return StatusCode::FAILURE;
     }
	
     // Make a vector for the cut results
     std::vector<int>* isMSVec = new std::vector<int>();

     // Loop over muons, set decisions
     for (xAOD::MuonContainer::const_iterator muIt = muons->begin(); muIt != muons->end(); ++muIt) {
       // has it a MS track?
       if ((*muIt)->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle) != NULL) isMSVec->push_back(1);
       else isMSVec->push_back(0);
     }     

     // Write decision to SG for access by downstream algs 
     std::string sgKey = m_sgPrefix + "isMS";

     if (evtStore()->contains<std::vector<int> >(sgKey)) {
       ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << sgKey << " which already exists. Please use a different key");
       return StatusCode::FAILURE;
     }
     CHECK(evtStore()->record(isMSVec, sgKey));
     
     return StatusCode::SUCCESS;
}
