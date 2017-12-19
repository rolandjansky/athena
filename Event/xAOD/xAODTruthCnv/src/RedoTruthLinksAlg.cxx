/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/errorcheck.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "RedoTruthLinksAlg.h"


using namespace std;

namespace xAODMaker {
    
    
    RedoTruthLinksAlg::RedoTruthLinksAlg( const string& name, ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc )
    {
    }
    
    
    StatusCode RedoTruthLinksAlg::initialize() {
        
	// initialize handles
	ATH_CHECK(m_truthLinkContainerKey.initialize());
	ATH_CHECK(m_linksOnlyTruthEventContainerKey.initialize());

        ATH_MSG_DEBUG("xAOD linksOnlyTruthEventContainer name = " << m_linksOnlyTruthEventContainerKey.key() );
        ATH_MSG_DEBUG("xAOD TruthParticleLinksContainer name = " << m_truthLinkContainerKey.key());


        return StatusCode::SUCCESS;
    }
    
    
    StatusCode RedoTruthLinksAlg::execute() {
        
	SG::WriteHandle<xAODTruthParticleLinkVector> truthLinkVec(m_truthLinkContainerKey);
	ATH_CHECK(truthLinkVec.record(std::make_unique<xAODTruthParticleLinkVector>()));
        
            
	SG::ReadHandle<xAOD::TruthEventContainer> xTruthEventContainer(m_linksOnlyTruthEventContainerKey);
	// validity check is only really needed for serial running. Remove when MT is only way.
	if (!xTruthEventContainer.isValid()) {
	  ATH_MSG_ERROR("Could not retrieve xAOD::TruthEventContainer with key:" << 
			m_linksOnlyTruthEventContainerKey.key());
	  return StatusCode::FAILURE;
	} else {
	  ATH_MSG_DEBUG( "Retrieved for reading xAOD::TruthEventContainer key: " <<
			 m_linksOnlyTruthEventContainerKey.key());
	}
	// Loop over events and particles
	for (const xAOD::TruthEvent* evt : *xTruthEventContainer) {
	  for (const auto& par : evt->truthParticleLinks()) {
	    if ( !par.isValid() ) {
	      // This can happen if particles have been thinned.
	      ATH_MSG_VERBOSE("Found invalid particle element link in TruthEvent"); //< @todo Use HepMC evt number?
	      continue;
	    }
	    // Create link between HepMC and xAOD truth
	    //truthLinkVec->push_back(new xAODTruthParticleLink(HepMcParticleLink((*par)->barcode(), evt->eventNumber()), par));
	    /// @todo AB: Truth particle links should only be made to the signal event... hence the 0. Right?
	    truthLinkVec->push_back(new xAODTruthParticleLink(HepMcParticleLink((*par)->barcode(), 0), par));
	  }
	}
        
        std::stable_sort(truthLinkVec->begin(), truthLinkVec->end(), SortTruthParticleLink());
        ATH_MSG_VERBOSE("Summarizing truth link size: " << truthLinkVec->size() );
        
        return StatusCode::SUCCESS;
    }
        
} // namespace xAODMaker
