/*
    Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TauJets_LepRMParticleThinning.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Dong Qichen  (qichen.dong@cern.ch)

#include "DerivationFrameworkInDet/TauJets_LepRMParticleThinning.h"
#include "xAODTau/TauxAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "StoreGate/ThinningHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <vector>
#include <string>

// Constructor
DerivationFramework::TauJets_LepRMParticleThinning::TauJets_LepRMParticleThinning(const std::string& t,
                                                                        const std::string& n,
                                                                        const IInterface* p ) :
base_class(t,n,p)
{
}

// Destructor
DerivationFramework::TauJets_LepRMParticleThinning::~TauJets_LepRMParticleThinning() {
}

// Athena initialize and finalize
StatusCode DerivationFramework::TauJets_LepRMParticleThinning::initialize()
{
    // Decide which collections need to be checked for ID TrackParticles
    ATH_MSG_VERBOSE("initialize() ...");
    ATH_CHECK( m_inDetSGKey.initialize (m_streamName) );
    ATH_MSG_INFO("Using " << m_inDetSGKey << " as the source collection for inner detector track particles");

    if (m_LepRMTauKey.key().empty()) 
    {
        ATH_MSG_FATAL("No tau collection provided for thinning.");
        return StatusCode::FAILURE;
    } 
    else 
    { 
        ATH_MSG_INFO("Inner detector track particles associated with objects in " << m_LepRMTauKey.key() << " will be retained in this format with the rest being thinned away");
    }
    ATH_CHECK(m_LepRMTauKey.initialize(m_streamName));

    //tautracks
    if (m_tauTracksSGKey.key().empty()) { 
        ATH_MSG_FATAL("No tau tracks collection provided for thinning.");
        return StatusCode::FAILURE;   
    } else {
        ATH_MSG_INFO("Tau track thinning requested; tau tracks with the SG key " << m_tauTracksSGKey.key() << " will be thinned if not associated with objects in " << m_LepRMTauKey.key());
        ATH_CHECK( m_tauTracksSGKey.initialize (m_streamName) ); 
    }

    // Set up the text-parsing machinery for selectiong the tau directly according to user cuts
    if (!m_selectionString.empty()) {
        ATH_CHECK(initializeParser(m_selectionString) );
    }

    ATH_CHECK(m_originalTauKey.initialize());
    return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::TauJets_LepRMParticleThinning::finalize()
{
    ATH_MSG_VERBOSE("finalize() ...");
    ATH_MSG_INFO("Processed "<< m_ntot_trks <<" tau tracks, "<< m_npass_trks<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntot_ID_trks <<" ID tracks, "<< m_npass_ID_trks<< " were retained ");
    ATH_MSG_INFO("Processed "<< m_ntot_taus <<" taus, "<< m_npass_taus<< " were retained ");
    ATH_CHECK( finalizeParser() );
    return StatusCode::SUCCESS;
}

// The thinning itself
StatusCode DerivationFramework::TauJets_LepRMParticleThinning::doThinning() const
{
    const EventContext& ctx = Gaudi::Hive::currentContext();
    // Retrieve main TrackParticle collection
    SG::ThinningHandle<xAOD::TrackParticleContainer> TrackParticles(m_inDetSGKey, ctx);
    if (!TrackParticles.isValid()) {
        ATH_MSG_ERROR("No tau collection with name " << m_inDetSGKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    SG::ThinningHandle<xAOD::TauTrackContainer> TauTracks(m_tauTracksSGKey, ctx);
    if (!TauTracks.isValid()) {
        ATH_MSG_ERROR("No tau collection with name " << m_tauTracksSGKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    SG::ThinningHandle<xAOD::TauJetContainer> LepRMTaus(m_LepRMTauKey,ctx);
    if (!LepRMTaus.isValid()) {
        ATH_MSG_ERROR("No tau collection with name " << m_LepRMTauKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }
    SG::ReadHandle<xAOD::TauJetContainer> OriTaus(m_originalTauKey,ctx);
    if (!OriTaus.isValid()) {
        ATH_MSG_ERROR("No tau collection with name " << m_originalTauKey.key() << " found in StoreGate!");
        return StatusCode::FAILURE;
    }

    //Thin tauJets_LepRM
    std::vector<int> tau_kine_mask =  m_parser->evaluateAsVector();
    std::vector<bool> tau_lep_remove_mask(OriTaus->size(),false);
    std::transform(OriTaus->cbegin(), OriTaus->cend(), LepRMTaus->cbegin(), tau_lep_remove_mask.begin(), 
        [](auto oritau, auto lep_removetau) -> bool {
            return oritau->nAllTracks() != lep_removetau->nAllTracks() || oritau->nClusters() != lep_removetau->nClusters();
        }
    );
    std::vector<bool> tau_mask(OriTaus->size(),false);
    std::transform(tau_lep_remove_mask.cbegin(), tau_lep_remove_mask.cend(), tau_kine_mask.cbegin(), tau_mask.begin(), 
        [](auto tau_lep_remove_pass, auto tau_kine_pass) -> bool
        {
            return tau_lep_remove_pass == 1 && tau_kine_pass == 1;
        } 
    );

    std::vector<const xAOD::TauJet*> LepRMTausAfterThinning;
    std::transform(tau_mask.cbegin(), tau_mask.cend(), LepRMTaus->cbegin(), std::back_inserter(LepRMTausAfterThinning), 
        [](bool mask, auto tau) -> const xAOD::TauJet*
        {
            return mask ? tau : nullptr;
        }
    );
    LepRMTausAfterThinning.erase(std::remove(LepRMTausAfterThinning.begin(), LepRMTausAfterThinning.end(), nullptr), LepRMTausAfterThinning.end());

    //Thin TauTrack_LepRM
    std::vector<bool> tau_track_mask(TauTracks->size(), false);
    std::vector<bool> id_track_mask(TrackParticles->size(), false);
    std::for_each(LepRMTausAfterThinning.cbegin(), LepRMTausAfterThinning.cend(), 
        [&](auto tau)
        {
            auto tau_track_links = tau->tauTrackLinks();
            std::for_each(tau_track_links.cbegin(), tau_track_links.cend(), 
                [&](auto link)
                {
                    if (link.isValid())
                    {
                        tau_track_mask[link.index()] = 1;
                        if (auto id_track_link = (*link)->trackLinks().at(0); id_track_link.isValid())
                        {
                            id_track_mask[id_track_link.index()] = 1;
                        }
                    }
                }
            );
        }
    );

    LepRMTaus.keep(tau_mask);
    TauTracks.keep(tau_track_mask);
    TrackParticles.keep(id_track_mask);

    m_ntot_taus     += OriTaus->size();
    m_ntot_trks     += TauTracks->size();
    m_ntot_ID_trks  += TrackParticles->size();
    m_npass_taus    += std::accumulate(tau_mask.begin(), tau_mask.end(),0);
    m_npass_trks    += std::accumulate(tau_track_mask.begin(), tau_track_mask.end(),0);
    m_npass_ID_trks += std::accumulate(id_track_mask.begin(), id_track_mask.end(),0);
    ATH_MSG_INFO(std::accumulate(tau_mask.begin(), tau_mask.end(),0) << " / " << OriTaus->size() << " taus were retained");

    return StatusCode::SUCCESS;

}