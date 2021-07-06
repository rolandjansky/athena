/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// DiTauTrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (James.Catmore@cern.ch)
// This is a trivial example of an implementation of a thinning tool
// which removes all ID tracks which do not pass a user-defined cut

#include "DerivationFrameworkInDet/DiTauTrackParticleThinning.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::DiTauTrackParticleThinning::DiTauTrackParticleThinning(const std::string& t,
                                                                            const std::string& n,
                                                                            const IInterface* p ) :
base_class(t,n,p)
{}

// Destructor
DerivationFramework::DiTauTrackParticleThinning::~DiTauTrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::DiTauTrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK( m_ditauKey.initialize () );
    ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_inDetSGKey.key() << "as the source collection for inner detector track particles");
    ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_ditauKey.key() << " will be retained in this format with the rest being thinned away");

    // Set up the text-parsing machinery for selectiong the ditau directly according to user cuts
    if (!m_selectionString.empty()) {
       ATH_CHECK(initializeParser(m_selectionString));
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::DiTauTrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    ATH_CHECK( finalizeParser() );
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::DiTauTrackParticleThinning::doThinning() const
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
    // ... ditaus
    SG::ReadHandle<xAOD::DiTauJetContainer> importedDiTaus(m_ditauKey,ctx);
    if (!importedDiTaus.isValid()) {
        ATH_MSG_ERROR("No ditau collection with name " << m_ditauKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nDiTaus(importedDiTaus->size());
    std::vector<const xAOD::DiTauJet*> ditauToCheck; ditauToCheck.clear();
    
    // Execute the text parser if requested
    if (!m_selectionString.empty()) {
        std::vector<int> entries =  m_parser->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nDiTaus != nEntries ) {
          ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used ditaus??");
            return StatusCode::FAILURE;
        } else {
          // identify which ditaus to keep for the thinning check
          for (unsigned int i=0; i<nDiTaus; ++i) if (entries[i]==1) ditauToCheck.push_back((*importedDiTaus)[i]);
        }
    }
    
    // Set elements in the mask to true if there is a corresponding ElementLink from a reconstructed object
    // ... ditaus
    if (m_selectionString=="") { // check all ditaus as user didn't provide a selection string
      for (xAOD::DiTauJetContainer::const_iterator ditauIt=importedDiTaus->begin(); ditauIt!=importedDiTaus->end(); ++ditauIt) {
        for (unsigned int i=0; i<(*ditauIt)->nTracks(); ++i) {
          int index = (*ditauIt)->trackLinks().at(i).index();
          mask[index] = true;
        }
        for (unsigned int i=0; i<(*ditauIt)->nIsoTracks(); ++i) {
          int index = (*ditauIt)->isoTrackLinks().at(i).index();
          mask[index] = true;
        }
      }
    } else { // check only ditaus passing user selection string
        for (std::vector<const xAOD::DiTauJet*>::iterator ditauIt = ditauToCheck.begin(); ditauIt!=ditauToCheck.end(); ++ditauIt) {
          for (unsigned int i=0; i<(*ditauIt)->nTracks(); ++i) {
              int index = (*ditauIt)->trackLinks().at(i).index();
              mask[index] = true;
          }
          for (unsigned int i=0; i<(*ditauIt)->nIsoTracks(); ++i) {
              int index = (*ditauIt)->isoTrackLinks().at(i).index();
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

    // Execute the thinning service based on the mask. Finish.
    importedTrackParticles.keep (mask);

    return StatusCode::SUCCESS;
}

