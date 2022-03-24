/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// JetLargeD0TrackParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Jackson Burzynski (jackson.carl.burzynski@cern.ch)
// This tool associates InDetLargeD0TrackParticles to jets using a
// user-defined DeltaR matching.
// n.b. The standard JetTrackParticleThinning tool will not work 
// for LargeD0 tracks due to the fact that they are not considered
// when running the ghost association.
//
// TODO: implement jet-origin correction under the assumption that
// the jet originates from a secondary vertex instead of the PV

#include "DerivationFrameworkLLP/JetLargeD0TrackParticleThinning.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::JetLargeD0TrackParticleThinning::JetLargeD0TrackParticleThinning(const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
base_class(t,n,p)
{
}

// Destructor
DerivationFramework::JetLargeD0TrackParticleThinning::~JetLargeD0TrackParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::JetLargeD0TrackParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_inDetSGKey << "as the source collection for inner detector track particles");
    if (m_jetKey.key().empty()) {
        ATH_MSG_FATAL("No jet collection provided for thinning.");
        return StatusCode::FAILURE;
    }
    ATH_CHECK( m_jetKey.initialize() );
    ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_jetKey.key() << " will be retained in this format with the rest being thinned away");

    // Set up the text-parsing machinery for selectiong the jet directly according to user cuts
    if (!m_selectionString.empty() || !m_trackSelectionString.empty()) {
       // order must match enum order EJetTrPThinningParser
       ATH_CHECK( initializeParser( {m_selectionString, m_trackSelectionString } ));
    }
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::JetLargeD0TrackParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot <<" tracks, "<< m_npass<< " were retained ");
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::JetLargeD0TrackParticleThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    
    // Retrieve main TrackParticle collection
    SG::ThinningHandle<xAOD::TrackParticleContainer> importedTrackParticles
      (m_inDetSGKey, ctx);
    SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_inDetSGKey,ctx);
    
    // Check the event contains tracks
    unsigned int nTracks = importedTrackParticles->size();
    if (nTracks==0) return StatusCode::SUCCESS;
    
    // Set up a mask with the same entries as the full TrackParticle collection
    std::vector<bool> mask;
    mask.assign(nTracks,false); // default: don't keep any tracks
    m_ntot += nTracks;
    
    // Retrieve containers
    // ... jets
    SG::ReadHandle<xAOD::JetContainer> importedJets(m_jetKey,ctx);
    if (!importedJets.isValid()) {
        ATH_MSG_ERROR("No jet collection with name " << m_jetKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    unsigned int nJets(importedJets->size());
    std::vector<const xAOD::Jet*> jetToCheck; jetToCheck.clear();
    
    // Execute the text parser if requested
    if (!m_selectionString.empty()) {
        std::vector<int> entries =  m_parser[kJetSelection]->evaluateAsVector();
        unsigned int nEntries = entries.size();
        // check the sizes are compatible
        if (nJets != nEntries ) {
        	ATH_MSG_ERROR("Sizes incompatible! Are you sure your selection string used jets??");
            return StatusCode::FAILURE;
        } else {
        	// identify which jets to keep for the thinning check
        	for (unsigned int i=0; i<nJets; ++i) if (entries[i]==1) jetToCheck.push_back((*importedJets)[i]);
        }
    }
    
    // Set elements in the mask to true if they are matched to a reconstructed object
    // ... jets
    static SG::AuxElement::ConstAccessor<std::vector<ElementLink<DataVector<xAOD::IParticle> > > > ghostTrackLRT ("GhostTrackLRT");

    if (m_selectionString.empty()) { // check all jets as user didn't provide a selection string
        for (xAOD::JetContainer::const_iterator jetIt=importedJets->begin(); jetIt!=importedJets->end(); ++jetIt) {
            const std::vector< ElementLink<DataVector<xAOD::IParticle> > > &jetTrackLinks = ghostTrackLRT( **jetIt ); 
            for (const auto &jetTrkIt : jetTrackLinks) {
              const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*jetTrkIt);
              int index = trackPart->index();
              mask[index] = true;
            }
        }
    } else { // check only jets passing user selection string
        for (xAOD::JetContainer::const_iterator jetIt=importedJets->begin(); jetIt!=importedJets->end(); ++jetIt) {
            const std::vector< ElementLink<DataVector<xAOD::IParticle> > > &jetTrackLinks = ghostTrackLRT( **jetIt ); 
            for (const auto &jetTrkIt : jetTrackLinks) {
              const xAOD::TrackParticle* trackPart = dynamic_cast<const xAOD::TrackParticle*>(*jetTrkIt);
              int index = trackPart->index();
              mask[index] = true;
            }
        }
    }

    // Apply a track selection string.
    if (!m_trackSelectionString.empty()) {
      std::vector<int> entries =  m_parser[kTrackThinning]->evaluateAsVector();
      unsigned int nEntries = entries.size();
      // check the sizes are compatible
      if (nTracks != nEntries ) {
        ATH_MSG_ERROR("Sizes incompatible! Are you sure your track selection string used tracks??");
        return StatusCode::FAILURE;
      } else {
        // identify which jets to keep for the thinning check
        for (unsigned int i=0; i<nEntries; ++i) {
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

