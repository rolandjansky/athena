/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// GenericObjectThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkTools/GenericObjectThinning.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::GenericObjectThinning::GenericObjectThinning(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
base_class(t,n,p),
m_selectionString(""),
m_ntot(0),
m_npass(0)
{
    declareProperty("SelectionString", m_selectionString);
}

// Destructor
DerivationFramework::GenericObjectThinning::~GenericObjectThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::GenericObjectThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_selectionString.empty()) {
        ATH_MSG_FATAL("No selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Selection string: " << m_selectionString);}
    
    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (!m_selectionString.empty()) {
       ATH_CHECK(initializeParser( m_selectionString) );
    }

    //check xAOD::InDetTrackParticle collection
    ATH_CHECK( m_SGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_SGKey << "as the source collection");
        
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::GenericObjectThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" objects, "<< m_npass<< " were retained ");
    ATH_CHECK(finalizeParser());
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::GenericObjectThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    
    // Retrieve main object collection
    SG::ThinningHandle<xAOD::IParticleContainer> particles (m_SGKey, ctx);
 
    // Check the event contains objects
    unsigned int nObjects = particles->size();
    if (nObjects==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full collection
    std::vector<bool> mask;
    mask.assign(nObjects,false); // default: don't keep any tracks
    m_ntot += nObjects;
    
    // Execute the text parser and update the mask
    if (m_parser) {
    	std::vector<int> entries =  m_parser->evaluateAsVector();
    	unsigned int nEntries = entries.size();
    	// check the sizes are compatible
    	if (nObjects != nEntries ) {
    		ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used the same collection as the input collection??");
            return StatusCode::FAILURE;
    	} else {
            // set mask
            for (unsigned int i=0; i<nObjects; ++i) if (entries[i]==1) mask[i]=true;
    	}
    }
    // Count the mask
    m_npass += std::count (mask.begin(), mask.end(), true);

    // Execute the thinning based on the mask. Finish.
    particles.keep (mask);
    
    return StatusCode::SUCCESS;
}

