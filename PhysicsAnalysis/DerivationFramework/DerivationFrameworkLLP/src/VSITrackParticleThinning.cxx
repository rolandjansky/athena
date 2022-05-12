/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// VSITrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Authors: Amber Roepe (amber.rebecca.roepe@cern.ch)
//          Jackson Burzynski (jackson.carl.burzynski@cern.ch)
// This is a trivial example of an implementation of a thinning tool
// which removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkLLP/VSITrackParticleThinning.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::VSITrackParticleThinning::VSITrackParticleThinning(const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
base_class(t,n,p)
{
}

// Destructor
DerivationFramework::VSITrackParticleThinning::~VSITrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::VSITrackParticleThinning::initialize()
{

    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );

    // Set up the text-parsing machinery for selectiong the dv directly according to user cuts
    if (!m_trackSelectionString.empty()) {
       // order must match enum order EJetTrPThinningParser
      ATH_CHECK( initializeParser( { m_selectionString, m_trackSelectionString } ));
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::VSITrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::VSITrackParticleThinning::doThinning() const
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
    
    // Set elements in the mask to true if associated with a DV
    for ( const auto trkIt: *importedTrackParticles ) {
      if(trkIt->isAvailable<char> ("is_svtrk_final")) {
        bool isSV = trkIt->auxdecor<char> ("is_svtrk_final");
        if ( isSV ) {
          int index = trkIt->index();
          mask[index] = true;
        }
      }
    }

    // Apply a track selection string.
    if (!m_trackSelectionString.empty()) {
      std::vector<int> entries =  m_parser[kdvTrackThinning]->evaluateAsVector();
      unsigned int nEntries = entries.size();
      // check the sizes are compatible
      if (nTracks != nEntries ) {
        ATH_MSG_ERROR("Sizes incompatible! Are you sure your track selection string used tracks??");
        return StatusCode::FAILURE;
      } 
      else {
        // identify which dvs to keep for the thinning check
        for (unsigned int i=0; i<nEntries; ++i) {
          if (!mask[i]) continue;
          if (!entries[i]) mask[i] = false;
        }
      }
    }
    

    // Count up the mask contents
    unsigned int n_pass=0;
    for (unsigned int i=0; i<nTracks; ++i) {
        if (mask[i]) ++n_pass;
    }
    m_npass += n_pass;
    

    // Execute the thinning service based on the mask. Finish.
    importedTrackParticles.keep (mask);


    return StatusCode::SUCCESS;
    
}

