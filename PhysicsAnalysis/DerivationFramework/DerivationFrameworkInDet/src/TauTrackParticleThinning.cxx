/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TauTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This is a trivial example of an implementation of a thinning tool
// which removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkInDet/TauTrackParticleThinning.h"
#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TauTrackParticleThinning::TauTrackParticleThinning(const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
base_class(t,n,p)
{
}

// Destructor
DerivationFramework::TauTrackParticleThinning::~TauTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TauTrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");
    if (m_tauKey.key().empty()) {
        ATH_MSG_FATAL("No tau collection provided for thinning.");
        return StatusCode::FAILURE;
    } else { ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_tauKey.key() << " will be retained in this format with the rest being thinned away");}
    ATH_CHECK(m_tauKey.initialize());
    if (m_doTauTracksThinning) {
       if (m_tauTracksSGKey.key().empty()) { 
           ATH_MSG_FATAL("No tau tracks collection provided for thinning, despite this option being requested.");
           return StatusCode::FAILURE;   
       } else {
           ATH_MSG_INFO("Tau track thinning requested; tau tracks with the SG key " << m_tauTracksSGKey.key() << " will be thinned if not associated with objects in " << m_tauKey.key());
           ATH_CHECK( m_tauTracksSGKey.initialize (m_streamName) ); 
       }
    }

    // Set up the text-parsing machinery for selectiong the tau directly according to user cuts
    if (!m_selectionString.empty()) {
	    ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
	    proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
	    proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
	    m_parser = std::make_unique<ExpressionParsing::ExpressionParser>(proxyLoaders);
	    m_parser->loadExpression(m_selectionString);
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TauTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    m_parser.reset();
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TauTrackParticleThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    
    // Retrieve main TrackParticle collection
    SG::ThinningHandle<xAOD::TrackParticleContainer> importedTrackParticles
      (m_inDetSGKey, ctx);
    
    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    if (nTracks==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full TrackParticle collection
    std::vector<bool> mask;
    mask.assign(nTracks,false); // default: don't keep any tracks
    m_ntot += nTracks;
    
    // Retrieve containers
    // ... taus
    SG::ReadHandle<xAOD::TauJetContainer> importedTaus(m_tauKey,ctx);
    if (!importedTaus.isValid()) {
        ATH_MSG_ERROR("No tau collection with name " << m_tauKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nTaus(importedTaus->size());
    std::vector<const xAOD::TauJet*> tauToCheck; tauToCheck.clear();
    
    // Execute the text parser if requested
    if (m_selectionString!="") {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nTaus != nEntries ) {
        	ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used taus??");
            return StatusCode::FAILURE;
        } else {
        	// identify which taus to keep for the thinning check
        	for (unsigned int i=0; i<nTaus; ++i) if (entries[i]==1) tauToCheck.push_back((*importedTaus)[i]);
        }
    }
    
    // Set elements in the mask to true if there is a corresponding ElementLink from a reconstructed object
    // ... taus
    DerivationFramework::TracksInCone trIC;
    if (m_selectionString=="") { // check all taus as user didn't provide a selection string
	    for (xAOD::TauJetContainer::const_iterator tauIt=importedTaus->begin(); tauIt!=importedTaus->end(); ++tauIt) {
                if (m_coneSize>0.0) trIC.select(*tauIt,m_coneSize,importedTrackParticles.cptr(),mask); // check tracks in a cone around the tau if req'd
            	for (unsigned int i=0; i<(*tauIt)->nTracks(); ++i) {
		  int index = xAOD::TauHelpers::trackParticleLinks(*tauIt, xAOD::TauJetParameters::TauTrackFlag::classifiedCharged).at(i).index();
                  mask[index] = true;
            	}
	    }
    } else { // check only taus passing user selection string
        for (std::vector<const xAOD::TauJet*>::iterator tauIt = tauToCheck.begin(); tauIt!=tauToCheck.end(); ++tauIt) {
            if (m_coneSize>0.0) trIC.select(*tauIt,m_coneSize,importedTrackParticles.cptr(),mask); // check tracks in a cone around the tau if req'd	
            for (unsigned int i=0; i<(*tauIt)->nTracks(); ++i) {
	      int index = xAOD::TauHelpers::trackParticleLinks(*tauIt, xAOD::TauJetParameters::TauTrackFlag::classifiedCharged).at(i).index();
              mask[index] = true;
            }
        }
    }
    
    // Count up the mask contents
    unsigned int n_pass=0;
    for (unsigned int i=0; i<nTracks; ++i) {
        if (mask[i]) ++n_pass;
    }
    m_npass += n_pass;

    // Execute the thinning service based on the mask. 
    importedTrackParticles.keep (mask);

   // Apply thinning to tau track collection if requested
   if( m_doTauTracksThinning ) {
      SG::ThinningHandle<xAOD::TauTrackContainer> importedTauTracks
      (m_tauTracksSGKey, ctx);
      if( importedTauTracks->size() == 0 ) {
         return StatusCode::SUCCESS;
      }    
      std::vector< bool > mask_tautracks( importedTauTracks->size(), false );
      
      for( const xAOD::TauJet* tau : tauToCheck ) {
         // Get all the associated charged tau tracks:
	 auto ttLinks = tau->tauTrackLinks(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged );
	 // Process the links:
	 for( const auto& ttLink : ttLinks ) {
            if( ! ttLink.isValid() ) {
               continue;
            }
            if( ttLink.dataID() != m_tauTracksSGKey.key() ) {
               ATH_MSG_FATAL( "Charged tau track does not come from "
                              "container \"" << m_tauTracksSGKey << "\"" );
               return StatusCode::FAILURE;
            }
            // If it is, set the mask for it:
            mask_tautracks.at( ttLink.index() ) = true;
         }
         // Select the tau tracks in a cone if it was requested (NOT RECOMMENDED):
	 if( m_coneSize > 0.0 ) {
	    trIC.select( tau, m_coneSize, importedTauTracks.cptr(), mask_tautracks );
	 }
       }

       importedTauTracks.keep(mask_tautracks);
	
   }
 
   return StatusCode::SUCCESS;

}

