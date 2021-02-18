/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpvElectronD0Tool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Mike Flowerdew (Michael.Flowerdew@cern.ch)
// Tool to write lepton d0 in a way accessible by derivation framwork tools
// Writes result to SG for later selection by string parser

#include "LongLivedParticleDPDMaker/RpvElectronD0Tool.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::RpvElectronD0Tool::RpvElectronD0Tool( const std::string& t,
							   const std::string& n,
							   const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_sgPrefix("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("SGPrefix", m_sgPrefix);
  }
 
// Destructor
DerivationFramework::RpvElectronD0Tool::~RpvElectronD0Tool() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::RpvElectronD0Tool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK(m_collName.initialize());
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
     SG::ReadHandle<xAOD::ElectronContainer> electrons(m_collName);
     if( !electrons.isValid() ) {
	ATH_MSG_ERROR("Couldn't retrieve e-gamma container with key: " << m_collName);
	return StatusCode::FAILURE;
     }
	
     // Write decision to SG for access by downstream algs 
     std::string sgKey(m_sgPrefix+"D0");
     SG::WriteHandle< std::vector<float> > d0vec(sgKey);
     ATH_CHECK(d0vec.record(std::make_unique< std::vector<float> >()));

     // Loop over electrons, set decisions
     for (xAOD::ElectronContainer::const_iterator eIt = electrons->begin(); eIt!=electrons->end(); ++eIt) {
       
       float d0 = 9e99;
       if (*eIt && (*eIt)->trackParticle()) d0 = (*eIt)->trackParticle()->d0();
       else ATH_MSG_WARNING("Problem accessing electron track!");
       
       d0vec->push_back(d0);
     
     }     
     
     return StatusCode::SUCCESS;

}

