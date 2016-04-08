/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpvElectronD0Tool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Mike Flowerdew (Michael.Flowerdew@cern.ch)
// Tool to write lepton d0 in a way accessible by derivation framwork tools
// Writes result to SG for later selection by string parser

#include "LongLivedParticleDPDMaker/RpvElectronD0Tool.h"
#include "xAODEgamma/ElectronContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::RpvElectronD0Tool::RpvElectronD0Tool( const std::string& t,
							   const std::string& n,
							   const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_collName("Electrons"),
  m_sgPrefix("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("CollectionName", m_collName);
    declareProperty("SGPrefix", m_sgPrefix);
  }
 
// Destructor
DerivationFramework::RpvElectronD0Tool::~RpvElectronD0Tool() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::RpvElectronD0Tool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     return StatusCode::SUCCESS;
}
StatusCode DerivationFramework::RpvElectronD0Tool::finalize()
{
     ATH_MSG_VERBOSE("finalize() ...");
     return StatusCode::SUCCESS;
}

// Augmentation
StatusCode DerivationFramework::RpvElectronD0Tool::addBranches() const
{

     // Retrieve data
     const xAOD::ElectronContainer* electrons =  evtStore()->retrieve< const xAOD::ElectronContainer >( m_collName );
     if( ! electrons ) {
	ATH_MSG_ERROR("Couldn't retrieve e-gamma container with key: " << m_collName);
	return StatusCode::FAILURE;
     }
	
     // Make a vector for the cut results
     std::vector<float>* d0vec = new std::vector<float>();

     // Loop over electrons, set decisions
     for (xAOD::ElectronContainer::const_iterator eIt = electrons->begin(); eIt!=electrons->end(); ++eIt) {
       
       float d0 = 9e99;
       if (*eIt && (*eIt)->trackParticle()) d0 = (*eIt)->trackParticle()->d0();
       else ATH_MSG_WARNING("Problem accessing electron track!");
       
       d0vec->push_back(d0);
     
     }     

     // Write decision to SG for access by downstream algs 
     std::string sgKey(m_sgPrefix+"D0");

     if (evtStore()->contains<std::vector<float> >(sgKey)) {
       ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << sgKey << " which already exists. Please use a different key");
       delete d0vec; // avoid mem leak
       return StatusCode::FAILURE;
     }
     CHECK(evtStore()->record(d0vec, sgKey));       
     
     return StatusCode::SUCCESS;

}

