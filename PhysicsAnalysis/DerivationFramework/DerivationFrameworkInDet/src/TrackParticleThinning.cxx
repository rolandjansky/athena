/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// Removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkInDet/TrackParticleThinning.h"
#include "AthenaKernel/IThinningSvc.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TrackParticleThinning::TrackParticleThinning(const std::string& t,
                                                                  const std::string& n,
                                                                  const IInterface* p ) :
AthAlgTool(t,n,p),
m_thinningSvc("ThinningSvc",n),
m_ntot(0),
m_npass(0),
m_inDetSGKey("InDetTrackParticles"),
m_selectionString(""),
m_and(false)
{
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("ThinningService", m_thinningSvc);
    declareProperty("InDetTrackParticlesKey", m_inDetSGKey);
    declareProperty("SelectionString", m_selectionString);
    declareProperty("ApplyAnd", m_and);
}

// Destructor
DerivationFramework::TrackParticleThinning::~TrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TrackParticleThinning::initialize()
{
    ATH_MSG_VERBOSE("initialize() ...");
    if (m_inDetSGKey=="") {
        ATH_MSG_FATAL("No inner detector track collection provided for thinning.");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");}
    
    if (m_selectionString=="") {
        ATH_MSG_FATAL("No inner detector track selection string provided!");
        return StatusCode::FAILURE;
    } else {ATH_MSG_INFO("Track thinning selection string: " << m_selectionString);}
    
    // Set up the text-parsing machinery for thinning the tracks directly according to user cuts
    if (m_selectionString!="") {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = new ExpressionParsing::ExpressionParser(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    if (m_parser) {
        delete m_parser;
        m_parser = 0;
    }
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TrackParticleThinning::doThinning() const
{
    
    // Retrieve main TrackParticle collection
    const xAOD::TrackParticleContainer* importedTrackParticles;
    if (evtStore()->retrieve(importedTrackParticles,m_inDetSGKey).isFailure()) {
        ATH_MSG_ERROR("No TrackParticle collection with name " << m_inDetSGKey << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    
    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    if (nTracks==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full TrackParticle collection
    std::vector<bool> mask;
    mask.assign(nTracks,false); // default: don't keep any tracks
    m_ntot += nTracks;
    
    // Execute the text parser and update the mask
    if (m_parser) {
    	std::vector<int> entries =  m_parser->evaluateAsVector();
    	unsigned int nEntries = entries.size();
    	// check the sizes are compatible
    	if (nTracks != nEntries ) {
    		ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used ID TrackParticles?");
            return StatusCode::FAILURE;
    	} else {
            // set mask
            for (unsigned int i=0; i<nTracks; ++i) if (entries[i]==1) mask[i]=true;
    	}
    }
    // Count the mask
    for (unsigned int i=0; i<nTracks; ++i) {
        if (mask[i]) ++m_npass;
    }
    
    // Execute the thinning service based on the mask. Finish.
    if (m_and) { 
    	if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::And).isFailure()) {
        	ATH_MSG_ERROR("Application of thinning service failed! ");
        	return StatusCode::FAILURE;
    	}
    }
    if (!m_and) {
        if (m_thinningSvc->filter(*importedTrackParticles, mask, IThinningSvc::Operator::Or).isFailure()) {
                ATH_MSG_ERROR("Application of thinning service failed! ");
                return StatusCode::FAILURE;
        }
    }
    
    return StatusCode::SUCCESS;
}

