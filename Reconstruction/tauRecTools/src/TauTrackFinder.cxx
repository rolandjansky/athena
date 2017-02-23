/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauTrackContainer.h"

#include "TauTrackFinder.h"
#include "tauRecTools/KineUtils.h"
#include "tauRecTools/TrackSort.h"


TauTrackFinder::TauTrackFinder(const std::string& name ) :
        TauRecToolBase(name),
        m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
        m_trackSelectorTool_tau(""),
        m_trackToVertexTool("Reco::TrackToVertex"),
        m_trackSelectorTool_tau_xAOD(""),
        m_z0maxDelta(1000),
        m_applyZ0cut(false),
        m_storeInOtherTrks(true),
        m_bypassSelector(false),
        m_bypassExtrapolator(false)
{
    declareProperty("MaxJetDrTau", m_maxJetDr_tau = 0.2);
    declareProperty("MaxJetDrWide", m_maxJetDr_wide = 0.4);
    declareProperty("TrackSelectorToolTau", m_trackSelectorTool_tau);
    declareProperty("TrackSelectorToolTauxAOD", m_trackSelectorTool_tau_xAOD);
    declareProperty("TrackParticleContainer", m_inputTrackParticleContainerName = "InDetTrackParticles");
    declareProperty("TauTrackParticleContainer", m_inputTauTrackContainerName = "TauTracks");
    declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
    declareProperty("TrackToVertexTool",m_trackToVertexTool);
    declareProperty("maxDeltaZ0wrtLeadTrk", m_z0maxDelta);
    declareProperty("removeTracksOutsideZ0wrtLeadTrk", m_applyZ0cut);
    declareProperty("StoreRemovedCoreWideTracksInOtherTracks", m_storeInOtherTrks = true);
    declareProperty("removeDuplicateCoreTracks", m_removeDuplicateCoreTracks = true);
    declareProperty("BypassSelector", m_bypassSelector = false);
    declareProperty("BypassExtrapolator", m_bypassExtrapolator = false);

    // initialize samplings
    m_EMSamplings = {CaloSampling::EME1, CaloSampling::EMB1};
    m_HadSamplings = {CaloSampling::TileBar1, CaloSampling::HEC1, CaloSampling::TileExt1};

}

TauTrackFinder::~TauTrackFinder() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::initialize() {

    // Get the TrackSelectorTool
    if (!retrieveTool(m_trackSelectorTool_tau)) return StatusCode::FAILURE;
    //if (!retrieveTool(m_trackSelectorTool_tau_xAOD)) return StatusCode::FAILURE;

    // Get the TJVA
    if (!retrieveTool(m_trackToVertexTool)) return StatusCode::FAILURE;
    if (!retrieveTool(m_caloExtensionTool)) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::finalize() {
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::eventInitialize() {
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::eventFinalize() {
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::execute(xAOD::TauJet& pTau) {

    ATH_MSG_VERBOSE("TauTrackFinder Executing");

    StatusCode sc;
    // get the track particle container from StoreGate
    const xAOD::TrackParticleContainer* trackParticleCont = 0;
    xAOD::TauTrackContainer* tauTrackCon = 0;

    //for tau trigger
    bool inTrigger = tauEventData()->inTrigger();
    if (inTrigger)   {
      ATH_CHECK(tauEventData()->getObject( "TrackContainer", trackParticleCont ));    
      ATH_CHECK(tauEventData()->getObject( "TauTrackContainer", tauTrackCon ));
    }

    if( !inTrigger || !trackParticleCont || sc.isFailure() ) {
        // try standard
        if (!openContainer(trackParticleCont, m_inputTrackParticleContainerName)) {
            if (!inTrigger) return StatusCode::FAILURE; // in offline we don't reconstruct tau candidates without having a track container
            else return StatusCode::SUCCESS; // we don't want stop trigger if there is no track container
        }
    }

    if( !inTrigger || !tauTrackCon || sc.isFailure() ) {
        // try standard
        if (!openContainer(tauTrackCon, m_inputTauTrackContainerName)) {
            if (!inTrigger) return StatusCode::FAILURE; // in offline we don't reconstruct tau candidates without having a track container
            else return StatusCode::SUCCESS; // we don't want stop trigger if there is no track container
        }
    }

    std::vector<const xAOD::TrackParticle*> tauTracks;
    std::vector<const xAOD::TrackParticle*> wideTracks;
    std::vector<const xAOD::TrackParticle*> otherTracks;

    const xAOD::Vertex* pVertex = pTau.vertexLink()!=0 ? (*pTau.vertexLink()) : NULL;
    // retrieve tracks wrt a vertex
    // as a vertex is used: tau origin / PV / beamspot / 0,0,0 (in this order, depending on availability)
    getTauTracksFromPV(pTau, *trackParticleCont, pVertex, tauTracks, wideTracks, otherTracks);


    this->resetDeltaZ0Cache();
    // remove core and wide tracks outside a maximal delta z0 wrt lead core track
    if (m_applyZ0cut) {
        this->removeOffsideTracksWrtLeadTrk(tauTracks, wideTracks, otherTracks, pVertex, m_z0maxDelta);
    }

    //check for tracks used in multiple taus
    xAOD::TauJetContainer* pContainer = tauEventData()->xAODTauContainer;
    if(pContainer==0){
      pContainer = static_cast<xAOD::TauJetContainer*> (pTau.container());
    }
    if(pContainer==0) {
      ATH_MSG_FATAL("Can't find tau Container");
      return StatusCode::FAILURE;
    }


    if(m_removeDuplicateCoreTracks){
      bool alreadyUsed = false;
      for (std::vector<const xAOD::TrackParticle*>::iterator track_it = tauTracks.begin(); track_it != tauTracks.end() ;)
        {
          alreadyUsed = false;
          
          //loop over all up-to-now core tracks
          for( const xAOD::TauTrack* tau_trk : (*tauTrackCon) ) {
        if(! tau_trk->flagWithMask( (1<<xAOD::TauJetParameters::TauTrackFlag::coreTrack) | (1<<xAOD::TauJetParameters::TauTrackFlag::passTrkSelector))) continue; //originally it was coreTrack&passTrkSelector
        if( (*track_it) == tau_trk->track()) alreadyUsed = true;
          }
          //if this track has already been used by another tau, don't associate it to this new one
          if(alreadyUsed) ATH_MSG_DEBUG( "Found Already Used track new, now removing: " << *track_it );
          if (alreadyUsed)    track_it = tauTracks.erase(track_it);
          else ++track_it;
        }
    }

    // associated track to tau candidate and calculate charge
    float charge = 0;
    for (unsigned int i = 0; i < tauTracks.size(); ++i) {
        const xAOD::TrackParticle* trackParticle = tauTracks.at(i);

        ATH_MSG_VERBOSE(name()     << " adding core track nr: " << i
                << " eta " << trackParticle->eta()
                << " phi " << trackParticle->phi()
        );
        charge += trackParticle->charge();

        xAOD::TauTrack* track = new xAOD::TauTrack();
        tauTrackCon->push_back(track);
        ElementLink<xAOD::TrackParticleContainer> linkToTrackParticle;
        linkToTrackParticle.toContainedElement(*trackParticleCont, trackParticle);
        track->addTrackLink(linkToTrackParticle);
        track->setP4(trackParticle->pt(), trackParticle->eta(), trackParticle->phi(), trackParticle->m());
        //track->setCharge(track->charge());
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::coreTrack, true);
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelector, true);
        // in case TrackClassifier is not run, still get sensible results
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged, true); 
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::unclassified, true); 
        //track->setFlag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelectionTight, m_trackSelectorTool_tau_xAOD->accept(trackParticle));
        ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
        linkToTauTrack.toContainedElement(*tauTrackCon, track);
        pTau.addTauTrackLink(linkToTauTrack);

        ATH_MSG_VERBOSE(name()     << " added core track nr: " << i
                << " eta " << pTau.track(i)->eta()
                << " phi " << pTau.track(i)->phi()
        );
    }
    // set the charge, which is defined by the core tau tracks only
    pTau.setCharge(charge);
    
    /// FIXME hide the logic to create element links inside xAODTau
    /// was
    // for (unsigned int i = 0; i < wideTracks.size(); ++i)
    //     details->addSeedCalo_wideTrk(trackParticleCont, wideTracks.at(i));
    for (unsigned int i = 0; i < wideTracks.size(); ++i) {
        const xAOD::TrackParticle* trackParticle = wideTracks.at(i);

        ATH_MSG_VERBOSE(name()     << " adding wide track nr: " << i
                << " eta " << trackParticle->eta()
                << " phi " << trackParticle->phi()
        );

        xAOD::TauTrack* track = new xAOD::TauTrack();
        tauTrackCon->push_back(track);
        ElementLink<xAOD::TrackParticleContainer> linkToTrackParticle;
        linkToTrackParticle.toContainedElement(*trackParticleCont, trackParticle);
        track->addTrackLink(linkToTrackParticle);
        track->setP4(trackParticle->pt(), trackParticle->eta(), trackParticle->phi(), trackParticle->m());
        //track->setCharge(track->charge());
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::wideTrack, true);
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelector, true);
        // in case TrackClassifier is not run, still get sensible results
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::classifiedIsolation, true); // for sake of trigger, reset in TauTrackClassifier
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::modifiedIsolationTrack, true); // for sake of trigger, reset in TauTrackClassifier
        track->setFlag(xAOD::TauJetParameters::TauTrackFlag::unclassified, true); 
        //track->setFlag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelectionTight, m_trackSelectorTool_tau_xAOD->accept(trackParticle));
        ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
        linkToTauTrack.toContainedElement(*tauTrackCon, track);
        pTau.addTauTrackLink(linkToTauTrack);

    }

    //These are set again in TauTrackClassifier
    pTau.setDetail(xAOD::TauJetParameters::nChargedTracks, (int) pTau.nTracks());
    pTau.setDetail(xAOD::TauJetParameters::nIsolatedTracks, (int) pTau.nTracks(xAOD::TauJetParameters::classifiedIsolation));

    /// was
    // for (unsigned int i = 0; i < otherTracks.size(); ++i)
    //     details->addOtherTrk(trackParticleCont, otherTracks.at(i));
    for (unsigned int i = 0; i < otherTracks.size(); ++i) {
        const xAOD::TrackParticle* trackParticle = otherTracks.at(i);

        ATH_MSG_VERBOSE(name()     << " adding other track nr: " << i
                << " eta " << trackParticle->eta()
                << " phi " << trackParticle->phi()
        );

        // bool accepted=m_trackSelectorTool_tau_xAOD->accept(trackParticle);
        // if(accepted){
          xAOD::TauTrack* track = new xAOD::TauTrack();
          tauTrackCon->push_back(track);
          ElementLink<xAOD::TrackParticleContainer> linkToTrackParticle;
          linkToTrackParticle.toContainedElement(*trackParticleCont, trackParticle);
          track->addTrackLink(linkToTrackParticle);
          track->setP4(trackParticle->pt(), trackParticle->eta(), trackParticle->phi(), trackParticle->m());
          float dR = track->p4().DeltaR(pTau.p4());
          if(dR<=0.2) track->setFlag(xAOD::TauJetParameters::TauTrackFlag::coreTrack, true);
          else track->setFlag(xAOD::TauJetParameters::TauTrackFlag::wideTrack, true);
          track->setFlag(xAOD::TauJetParameters::TauTrackFlag::unclassified, true); 
          // track->setFlag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelectionTight, accepted);
          ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
          linkToTauTrack.toContainedElement(*tauTrackCon, track);
          pTau.addTauTrackLink(linkToTauTrack);
        // }
    }


    ATH_MSG_DEBUG("numTrack: " << "/" << pTau.nTracks());
    ATH_MSG_DEBUG("charge: " << "/" << pTau.charge());

    // extrapolate core tracks to calorimeter surface
    // store information only in ExtraDetailsContainer

    if(!m_bypassExtrapolator)
      {
        sc = extrapolateToCaloSurface(pTau);
        if (sc.isFailure() && !sc.isRecoverable()) {
          ATH_MSG_ERROR("couldn't extrapolate tracks to calo surface");
          return StatusCode::FAILURE;
        }
      }
    
    return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
TauTrackFinder::TauTrackType TauTrackFinder::tauTrackType( const xAOD::TauJet& pTau,
        const xAOD::TrackParticle& trackParticle,
        const xAOD::Vertex* primaryVertex)
{
    //ATH_MSG_VERBOSE("tau axis:" << pTau.hlv().perp()<< " " << pTau.hlv().eta() << " " << pTau.hlv().phi()  << " " << pTau.hlv().e() );
    double dR = Tau1P3PKineUtils::deltaR(pTau.eta(),pTau.phi(),trackParticle.eta(),trackParticle.phi());

    if (dR > m_maxJetDr_wide) return NotTauTrack;

    bool goodTrack = true;
    if(!m_bypassSelector)
      goodTrack = m_trackSelectorTool_tau->decision(trackParticle, primaryVertex);
    
    if (goodTrack) {
      if (dR > m_maxJetDr_tau)
        return TauTrackWide;
      else
        return TauTrackCore;
    } else
      return TauTrackOther;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void TauTrackFinder::getTauTracksFromPV( const xAOD::TauJet& pTau,
        const xAOD::TrackParticleContainer& trackParticleCont,
        const xAOD::Vertex* primaryVertex,
        std::vector<const xAOD::TrackParticle*> &tauTracks,
        std::vector<const xAOD::TrackParticle*> &wideTracks,
        std::vector<const xAOD::TrackParticle*> &otherTracks)
{
    for (xAOD::TrackParticleContainer::const_iterator tpcItr = trackParticleCont.begin(); tpcItr != trackParticleCont.end(); ++tpcItr) {
        const xAOD::TrackParticle *trackParticle = *tpcItr;

        TauTrackType type = tauTrackType(pTau, *trackParticle, primaryVertex);

        if (type == TauTrackCore)
            tauTracks.push_back(trackParticle);
        else if (type == TauTrackWide)
            wideTracks.push_back(trackParticle);
        else if (type == TauTrackOther)
            otherTracks.push_back(trackParticle);

    }
    std::sort(tauTracks.begin(), tauTracks.end(), TrackSort());
    std::sort(wideTracks.begin(), wideTracks.end(), TrackSort());
    std::sort(otherTracks.begin(), otherTracks.end(), TrackSort());
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::extrapolateToCaloSurface(xAOD::TauJet& pTau) {

    //apparently unused: const int numOfsampEM = 4;

    Trk::TrackParametersIdHelper parsIdHelper;

    //    for (unsigned int itr = 0; itr < 10 && itr < pTau.nAllTracks(); ++itr) {
    
    for( xAOD::TauTrack* tauTrack : pTau.allTracks() ) {
        const xAOD::TrackParticle *orgTrack = tauTrack->track();
        
        if( !orgTrack ) continue;

        // set default values
        float etaEM = -10.0;
        float phiEM = -10.0;
        float etaHad = -10.0;
        float phiHad = -10.0;

        // get the extrapolation into the calo
        ATH_MSG_DEBUG( "Try extrapolation of track with pt = " << orgTrack->pt() 
                       << ", eta " << orgTrack->eta() 
                       << ", phi" << orgTrack->phi() );

        const Trk::CaloExtension* caloExtension = 0;
        if (not m_caloExtensionTool->caloExtension(*orgTrack,caloExtension)
            or caloExtension->caloLayerIntersections().empty() )
        { 
            ATH_MSG_DEBUG("Track extrapolation failed");
        }
        else {
            ATH_MSG_DEBUG("Scanning samplings");
            bool validECal = false;
            bool validHCal = false;
            for( auto cur : caloExtension->caloLayerIntersections() ){
                ATH_MSG_DEBUG("Sampling " << parsIdHelper.caloSample(cur->cIdentifier()) );
                
                // only use entry layer
                if( not parsIdHelper.isEntryToVolume(cur->cIdentifier()) ) continue;

                CaloSampling::CaloSample sample = parsIdHelper.caloSample(cur->cIdentifier());

                // ECal
                if( not validECal and m_EMSamplings.count(sample))
                {
                    validECal = true;
                    etaEM = cur->position().eta();
                    phiEM = cur->position().phi();
                    ATH_MSG_DEBUG("Extrapolated to ECal layer " << sample);
                }

                 // HCal
                if( not validHCal and m_HadSamplings.count(sample))
                {
                    validHCal = true;
                    etaHad = cur->position().eta();
                    phiHad = cur->position().phi();
                    ATH_MSG_DEBUG("Extrapolated to HCal layer " << sample);
                }
                if( validECal and validHCal ) break;
            }
            // EM failure warn if within acceptance 
            if( not validECal and abs(orgTrack->pt()) < 2.48 ){
                ATH_MSG_DEBUG("Failed extrapolation to ECal");
            }
            // Had failure warn if enough pt to reach HCal
            if( not validHCal and orgTrack->pt() > 2000. ){
                ATH_MSG_DEBUG("Failed extrapolation to HCal");
            }

            ATH_MSG_DEBUG( "Extrapolated track with eta=" << orgTrack->eta()
                            << " phi="<<orgTrack->phi()
                            << " to ECal eta=" << etaEM 
                            << " phi="<< phiEM
                            << " HCal eta=" << etaHad 
                            << " phi="<< phiHad
                            );
        }
        tauTrack->setDetail(xAOD::TauJetParameters::CaloSamplingEtaEM, etaEM);
        tauTrack->setDetail(xAOD::TauJetParameters::CaloSamplingPhiEM, phiEM);
        tauTrack->setDetail(xAOD::TauJetParameters::CaloSamplingEtaHad, etaHad);
        tauTrack->setDetail(xAOD::TauJetParameters::CaloSamplingPhiHad, phiHad);

        
        // //---------------------------------------------------------------------
        // // XXX commenting out old extrapolation using setEtaStrip 
        // //---------------------------------------------------------------------

        //  // get the extrapolation into the calo
        //  const Trk::CaloExtension* caloExtension = 0;
        //  if( !m_caloExtensionTool->caloExtension(*orgTrack,caloExtension) || caloExtension->caloLayerIntersections().empty() ) continue;
        //
        //  // loop over calo layers
        //  for( auto cur = caloExtension->caloLayerIntersections().begin(); cur != caloExtension->caloLayerIntersections().end() ; ++cur ){
        //    
        //      // only use entry layer
        //      if( !parsIdHelper.isEntryToVolume((*cur)->cIdentifier()) ) continue;
        //
        //      CaloSampling::CaloSample sample = parsIdHelper.caloSample((*cur)->cIdentifier());
        //
        //      if( sample == CaloSampling::EME1 || sample == CaloSampling::EMB1 ){
        //          tauTrack->setEtaStrip((*cur)->position().eta());
        //          tauTrack->setPhiStrip((*cur)->position().phi());
        //          break;
        //      }
        //  }


        // //---------------------------------------------------------------------
        // // Extrapolate to all layers
        // //---------------------------------------------------------------------
        // const DataVector< const Trk::TrackParameters >* pTrk = m_trackToCalo->getParametersInCalo(*orgTrack, Trk::pion, Trk::alongMomentum); //FIXME

        // //---------------------------------------------------------------------
        // // Calculate eta, phi impact point at calorimeter layers EM 0,1,2,3
        // //---------------------------------------------------------------------
        // double eta_extrapol[4];
        // double phi_extrapol[4];

        // for (int i = 0; i < numOfsampEM; ++i) {
        //     eta_extrapol[i] = -11111.;
        //     phi_extrapol[i] = -11111.;
        // }

        // // XXX commenting this out as long as it's not clear whether these variables will be stored in xAOD::TauJet
        // // if (pTrk && (*pTrk)[IExtrapolateToCaloTool::PreSampler]) {
        // //     eta_extrapol[0] = (*pTrk)[IExtrapolateToCaloTool::PreSampler]->position().eta();
        // //     phi_extrapol[0] = (*pTrk)[IExtrapolateToCaloTool::PreSampler]->position().phi();
        // // }

        // if (pTrk && (*pTrk)[IExtrapolateToCaloTool::Strips]) {
        //     eta_extrapol[1] = (*pTrk)[IExtrapolateToCaloTool::Strips]->position().eta();
        //     phi_extrapol[1] = (*pTrk)[IExtrapolateToCaloTool::Strips]->position().phi();

        // if (msgLvl(MSG::VERBOSE)) 
        //   { //only if desired msg level is requested
        //     ATH_MSG_VERBOSE(name() << " extrapolation in strip layer : "  
        //             << " track nr " << itr
        //             << " impact point eta " << eta_extrapol[1]
        //             << " impact point phi " << phi_extrapol[1]
        //             );
        //   }
    
        // pTau.setTrackEtaStrip( itr,  (*pTrk)[IExtrapolateToCaloTool::Strips]->position().eta() );
        // pTau.setTrackPhiStrip( itr,  (*pTrk)[IExtrapolateToCaloTool::Strips]->position().phi() );
        
        // if (msgLvl(MSG::VERBOSE)) 
        //   { //only if desired msg level is requested
        //     ATH_MSG_VERBOSE(name() << " extrapolation in strip layer stored in tau : "  
        //             << " track nr " << itr
        //             << " impact point eta " << pTau.trackEtaStrip(itr)
        //             << " impact point phi " << pTau.trackPhiStrip(itr)
        //             );
        //   }
    
        // }

        // XXX commenting this out as long as it's not clear whether these variables will be stored in xAOD::TauJet
        // if (pTrk && (*pTrk)[IExtrapolateToCaloTool::Middle]) {
        //     eta_extrapol[2] = (*pTrk)[IExtrapolateToCaloTool::Middle]->position().eta();
        //     phi_extrapol[2] = (*pTrk)[IExtrapolateToCaloTool::Middle]->position().phi();
        // }

        // if (pTrk && (*pTrk)[IExtrapolateToCaloTool::Back]) {
        //     eta_extrapol[3] = (*pTrk)[IExtrapolateToCaloTool::Back]->position().eta();
        //     phi_extrapol[3] = (*pTrk)[IExtrapolateToCaloTool::Back]->position().phi();
        // }


        // for (int i = 0; i < numOfsampEM; ++i) {
        //     pExtraDetails->etaLooseTrkCaloSamp()[itr][i] = eta_extrapol[i];
        //     pExtraDetails->phiLooseTrkCaloSamp()[itr][i] = phi_extrapol[i];
        // }

        // // in the past this was filled by tau1P3PTrackMatchCalo
        // for (int i = 0; i < numOfsampEM; ++i) {
        //     pExtraDetails->etaTrkCaloSamp()[itr][i] = eta_extrapol[i];
        //     pExtraDetails->phiTrkCaloSamp()[itr][i] = phi_extrapol[i];
        // }

        //if (pTrk) delete pTrk;

        // XXX commenting this out as long as it's not clear whether these variables will be stored in xAOD::TauJet
        // if (msgLvl(MSG::VERBOSE)) { //only if desired msg level is requested
        //   for (int i = 0; i < numOfsampEM; ++i) {
        //     ATH_MSG_VERBOSE(name() << " extrapolation for loose trk in samp : " << i
        //                     << " track nr " << itr
        //                     << " impact point eta " << pExtraDetails->etaLooseTrkCaloSamp()[itr][i]
        //                     << " impact point phi " << pExtraDetails->phiLooseTrkCaloSamp()[itr][i]
        //                     );
        //     ATH_MSG_VERBOSE(name() << " extrapolation in samp : " << i
        //                     << " track nr " << itr
        //                     << " impact point eta " << pExtraDetails->etaTrkCaloSamp()[itr][i]
        //                     << " impact point phi " << pExtraDetails->phiTrkCaloSamp()[itr][i]
        //                     );
        //   }
        // }

    }

    return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void TauTrackFinder::removeOffsideTracksWrtLeadTrk(std::vector<const Rec::TrackParticle*> &tauTracks,
                                       std::vector<const Rec::TrackParticle*> &wideTracks,
                                       std::vector<const Rec::TrackParticle*> &otherTracks,
                                       const Trk::RecVertex* tauOrigin,
                                       double maxDeltaZ0)
{
    float MAX=1e5;
    this->resetDeltaZ0Cache();

    // need at least one core track to have a leading trk to compare with
    if (tauTracks.size()<1) return;

    //check if position is available for origin
    /** FIXME: This causes compilation error after eigen migration
    if (tauOrigin)
        if (!tauOrigin->position())
            tauOrigin = 0;
    */

    // get lead trk parameters
    const Rec::TrackParticle *leadTrack = tauTracks.at(0);
    float z0_leadTrk = getZ0(leadTrack, tauOrigin);

    if (z0_leadTrk > MAX-1) return; // bad lead trk -> do nothing

    ATH_MSG_VERBOSE("before z0 cut: #coreTracks=" << tauTracks.size() << ", #wideTracks=" << wideTracks.size() << ", #otherTracks=" << otherTracks.size());

    std::vector<const Rec::TrackParticle*>::iterator itr;

    // check core tracks
    // skip leading track, because it is the reference
    itr = tauTracks.begin()+1;
    while (itr!=tauTracks.end()) {
        float z0 = getZ0(*itr, tauOrigin);
        float deltaZ0=z0 - z0_leadTrk;

        ATH_MSG_VERBOSE("core Trks: deltaZ0= " << deltaZ0);
        m_vDeltaZ0coreTrks.push_back(deltaZ0);

        if ( fabs(deltaZ0) < maxDeltaZ0 ) {++itr;}
        else {
            if (m_storeInOtherTrks) otherTracks.push_back(*itr);
            itr = tauTracks.erase(itr); //remove from core track collection
        }
    }

    // check wide tracks
    itr = wideTracks.begin();
    while (itr!=wideTracks.end()) {
        float z0 = getZ0(*itr, tauOrigin);
        float deltaZ0=z0 - z0_leadTrk;

        ATH_MSG_VERBOSE("wide Trks: deltaZ0= " << deltaZ0);
        m_vDeltaZ0wideTrks.push_back(deltaZ0);

        if ( fabs(deltaZ0) < maxDeltaZ0 ) { ++itr; }
        else {
            if (m_storeInOtherTrks) otherTracks.push_back(*itr);
            itr = wideTracks.erase(itr); //remove from wide track collection
        }
    }

    ATH_MSG_VERBOSE("after z0 cut: #coreTracks=" << tauTracks.size() << ", #wideTracks=" << wideTracks.size() << ", #otherTracks=" << otherTracks.size());

    // sort again
    std::sort(tauTracks.begin(), tauTracks.end(), TrackSort());
    std::sort(wideTracks.begin(), wideTracks.end(), TrackSort());
    std::sort(otherTracks.begin(), otherTracks.end(), TrackSort());
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void TauTrackFinder::removeOffsideTracksWrtLeadTrk(std::vector<const xAOD::TrackParticle*> &tauTracks,
                           std::vector<const xAOD::TrackParticle*> &wideTracks,
                           std::vector<const xAOD::TrackParticle*> &otherTracks,
                           const xAOD::Vertex* tauOrigin,
                           double maxDeltaZ0)
{
    float MAX=1e5;
    this->resetDeltaZ0Cache();

    // need at least one core track to have a leading trk to compare with
    if (tauTracks.size()<1) return;

    //check if position is available for origin
    /** FIXME: This causes compilation error after eigen migration
    if (tauOrigin)
        if (!tauOrigin->position())
            tauOrigin = 0;
    */

    // get lead trk parameters
    const xAOD::TrackParticle *leadTrack = tauTracks.at(0);
    float z0_leadTrk = getZ0(leadTrack, tauOrigin);

    if (z0_leadTrk > MAX-1) return; // bad lead trk -> do nothing

    ATH_MSG_VERBOSE("before z0 cut: #coreTracks=" << tauTracks.size() << ", #wideTracks=" << wideTracks.size() << ", #otherTracks=" << otherTracks.size());

    std::vector<const xAOD::TrackParticle*>::iterator itr;

    // check core tracks
    // skip leading track, because it is the reference
    itr = tauTracks.begin()+1;
    while (itr!=tauTracks.end()) {
        float z0 = getZ0(*itr, tauOrigin);
        float deltaZ0=z0 - z0_leadTrk;

        ATH_MSG_VERBOSE("core Trks: deltaZ0= " << deltaZ0);
        m_vDeltaZ0coreTrks.push_back(deltaZ0);

        if ( fabs(deltaZ0) < maxDeltaZ0 ) {++itr;}
        else {
            if (m_storeInOtherTrks) otherTracks.push_back(*itr);
            itr = tauTracks.erase(itr); //remove from core track collection
        }
    }

    // check wide tracks
    itr = wideTracks.begin();
    while (itr!=wideTracks.end()) {
        float z0 = getZ0(*itr, tauOrigin);
        float deltaZ0=z0 - z0_leadTrk;

        ATH_MSG_VERBOSE("wide Trks: deltaZ0= " << deltaZ0);
        m_vDeltaZ0wideTrks.push_back(deltaZ0);

        if ( fabs(deltaZ0) < maxDeltaZ0 ) { ++itr; }
        else {
            if (m_storeInOtherTrks) otherTracks.push_back(*itr);
            itr = wideTracks.erase(itr); //remove from wide track collection
        }
    }

    ATH_MSG_VERBOSE("after z0 cut: #coreTracks=" << tauTracks.size() << ", #wideTracks=" << wideTracks.size() << ", #otherTracks=" << otherTracks.size());

    // sort again
    std::sort(tauTracks.begin(), tauTracks.end(), TrackSort());
    std::sort(wideTracks.begin(), wideTracks.end(), TrackSort());
    std::sort(otherTracks.begin(), otherTracks.end(), TrackSort());
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
float TauTrackFinder::getZ0(const Rec::TrackParticle* track, const Trk::RecVertex* vertex)
{
    float MAX=1e5;

    if (!track) return MAX;
    if (!track->measuredPerigee()->covariance()) {
         ATH_MSG_WARNING("Bad track; can't find perigee at vertex.");
         return MAX;
    }

    const Trk::Perigee* perigee = 0;
    if (vertex) perigee = m_trackToVertexTool->perigeeAtVertex(*track, vertex->position());
    else        perigee = m_trackToVertexTool->perigeeAtVertex(*track); //will use beamspot or 0,0,0 instead

    if (!perigee) {
        ATH_MSG_WARNING("Bad track; can't find perigee at vertex.");
        return MAX;
    }

    float z0 = perigee->parameters()[Trk::z0];

    delete perigee; //cleanup necessary to prevent mem leak

    return z0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
float TauTrackFinder::getZ0(const xAOD::TrackParticle* track, const xAOD::Vertex* vertex)
{
    float MAX=1e5;

    if (!track) return MAX;

    const Trk::Perigee* perigee = 0;
    if (vertex) perigee = m_trackToVertexTool->perigeeAtVertex(*track, vertex->position());
    else        perigee = m_trackToVertexTool->perigeeAtVertex(*track); //will use beamspot or 0,0,0 instead

    if (!perigee) {
        ATH_MSG_WARNING("Bad track; can't find perigee at vertex.");
        return MAX;
    }

    float z0 = perigee->parameters()[Trk::z0];

    delete perigee; //cleanup necessary to prevent mem leak

    return z0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void TauTrackFinder::getDeltaZ0Values(std::vector<float>& vDeltaZ0coreTrks, std::vector<float>& vDeltaZ0wideTrks)
{
  vDeltaZ0coreTrks.clear();
  vDeltaZ0coreTrks = m_vDeltaZ0coreTrks;

  vDeltaZ0wideTrks.clear();
  vDeltaZ0wideTrks = m_vDeltaZ0wideTrks;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void TauTrackFinder::resetDeltaZ0Cache()
{
    m_vDeltaZ0coreTrks.clear();
    m_vDeltaZ0wideTrks.clear();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Helpers
template <class T>
bool TauTrackFinder::openContainer(T* &container, std::string containerName, bool printFATAL) {
    StatusCode sc = evtStore()->retrieve(container, containerName);
    if (sc.isFailure() || !container) {
      if (printFATAL) ATH_MSG_FATAL("Container (" << containerName << ") not found in StoreGate");
      return 0;
    }
    return container;
}

template <class T>
bool TauTrackFinder::retrieveTool(T & tool) {
    if (tool.retrieve().isFailure()) {
        ATH_MSG_FATAL("Failed to retrieve tool " << tool);
        return false;
    } else {
        ATH_MSG_VERBOSE("Retrieved tool " << tool);
    }
    return true;
}
#endif
