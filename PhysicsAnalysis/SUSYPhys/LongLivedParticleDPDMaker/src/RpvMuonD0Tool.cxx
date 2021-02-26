/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RpvMuonD0Tool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Mike Flowerdew (Michael.Flowerdew@cern.ch)
// Tool to write lepton d0 in a way accessible by derivation framwork tools
// Writes result to SG for later selection by string parser

#include "LongLivedParticleDPDMaker/RpvMuonD0Tool.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::RpvMuonD0Tool::RpvMuonD0Tool( const std::string& t,
						   const std::string& n,
						   const IInterface* p ) :
  AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
  }
 
// Destructor
DerivationFramework::RpvMuonD0Tool::~RpvMuonD0Tool() {
} 

// Athena initialize and finalize
StatusCode DerivationFramework::RpvMuonD0Tool::initialize()
{
     ATH_MSG_VERBOSE("initialize() ...");
     ATH_CHECK(m_collNameKey.initialize());
     ATH_CHECK(m_collNameD0Key.initialize());
     ATH_CHECK(m_collNameIsCombKey.initialize());
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
     SG::ReadHandle<xAOD::MuonContainer> muons(m_collNameKey);
     if( !muons.isValid() ) {
	ATH_MSG_ERROR("Couldn't retrieve muon container with key: " << m_collName);
	return StatusCode::FAILURE;
     }
	
     // Make a vector for the cut results
     // Write decision to SG for access by downstream algs 
     SG::WriteHandle< std::vector<float> > d0vec(m_collNameD0Key);
     ATH_CHECK(d0vec.record(std::make_unique< std::vector<float> >()));

     SG::WriteHandle< std::vector<int> > isCombinedVec(m_collNameIsCombKey);
     ATH_CHECK(isCombinedVec.record(std::make_unique< std::vector<int> >()));


     // Loop over muons, set decisions
     for (xAOD::MuonContainer::const_iterator muIt = muons->begin(); muIt!=muons->end(); ++muIt) {
       
       float d0 = 9e99;
	 
       // Alternative: (*muIt)->trackParticle( xAOD::Muon::InnerDetectorTrackParticle ) ?
       if (*muIt && (*muIt)->primaryTrackParticle()) d0 = (*muIt)->primaryTrackParticle()->d0();
       else ATH_MSG_WARNING("Problem accessing muon track!");	 
       
       d0vec->push_back(d0);
       //// is it a combined muon?  /// NEW - also see if it has a bad chisq for ID trk match - if so, treat it as standalone (i.e. don't apply d0 cut)
       bool isGoodCombined = false;
       
       if ((*muIt)->muonType()==xAOD::Muon::Combined) {
	 float chi2 = 0.;
	 if (!(*muIt)->parameter(chi2, xAOD::Muon::msInnerMatchChi2)) ATH_MSG_WARNING("Problem accessing muon chisq!");
	 int dof = 1;
	 if (!(*muIt)->parameter(dof,xAOD::Muon::msInnerMatchDOF)) ATH_MSG_WARNING("Problem accessing muon DoF!");
	 if (dof == 0) dof = 1;
	 if (chi2/float(dof) < 5.)  isGoodCombined = true;  ///  MS <==> ID match - require good chisq
       }
       if (isGoodCombined) {
	   isCombinedVec->push_back(1); 
       } else {
	 isCombinedVec->push_back(0);  /// either not combined, or bad chisq.
       }     
     }
     
     return StatusCode::SUCCESS;
}

