/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauTrackContainer.h"

#include "TauTrackFinder.h"
#include "tauRecTools/KineUtils.h"
#include "tauRecTools/TrackSort.h"


TauTrackFinder::TauTrackFinder(const std::string& name ) :
        TauRecToolBase(name) {
    m_EMSamplings = {CaloSampling::EME1, CaloSampling::EMB1};
    m_HadSamplings = {CaloSampling::TileBar1, CaloSampling::HEC1, CaloSampling::TileExt1};
}

TauTrackFinder::~TauTrackFinder() {
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::initialize() {

    // retrieve tools
    ATH_CHECK( m_trackSelectorTool_tau.retrieve() );
    ATH_CHECK( m_trackToVertexTool.retrieve() );
    ATH_CHECK( m_caloExtensionTool.retrieve() );
    
    // initialize ReadHandleKey
    // allow empty for trigger
    ATH_CHECK( m_trackPartInputContainer.initialize(SG::AllowEmpty) );
    // use CaloExtensionTool when key is empty 
    ATH_CHECK( m_ParticleCacheKey.initialize(SG::AllowEmpty) );

    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::finalize() {
    return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode TauTrackFinder::executeTrackFinder(xAOD::TauJet& pTau, xAOD::TauTrackContainer& tauTrackCon, const xAOD::TrackParticleContainer* trackContainer) const {
  
  std::vector<const xAOD::TrackParticle*> tauTracks;
  std::vector<const xAOD::TrackParticle*> wideTracks;
  std::vector<const xAOD::TrackParticle*> otherTracks;
  
  const xAOD::TrackParticleContainer* trackParticleCont = nullptr; 
  
  if (! m_trackPartInputContainer.empty()) { // MT version of trigger or offline
    SG::ReadHandle<xAOD::TrackParticleContainer> trackPartInHandle( m_trackPartInputContainer );
    if (!trackPartInHandle.isValid()) {
      ATH_MSG_ERROR ("Could not retrieve HiveDataObj with key " << trackPartInHandle.key());
      return StatusCode::FAILURE;
    }
    trackParticleCont = trackPartInHandle.cptr();
  }
  else { // coule be possible in trigger
    if (trackContainer != nullptr) { 
      trackParticleCont = trackContainer;
    }
    else {
      ATH_MSG_WARNING("No track container found");
      return StatusCode::FAILURE;
    }
  }

  // get the primary vertex
  const xAOD::Vertex* pVertex = pTau.vertexLink()!=0 ? (*pTau.vertexLink()) : NULL;

  // retrieve tracks wrt a vertex                                                                                                                              
  // as a vertex is used: tau origin / PV / beamspot / 0,0,0 (in this order, depending on availability)                                                        
  getTauTracksFromPV(pTau, *trackParticleCont, pVertex, tauTracks, wideTracks, otherTracks);

  // remove core and wide tracks outside a maximal delta z0 wrt lead core track                                                                                
  if (m_applyZ0cut) {
    this->removeOffsideTracksWrtLeadTrk(tauTracks, wideTracks, otherTracks, pVertex, m_z0maxDelta);
  }

  if(m_removeDuplicateCoreTracks){
    bool alreadyUsed = false;
    for (std::vector<const xAOD::TrackParticle*>::iterator track_it = tauTracks.begin(); track_it != tauTracks.end() ;)
      {
	alreadyUsed = false;
	//loop over all up-to-now core tracks	
	for( const xAOD::TauTrack* tau_trk : (tauTrackCon) ) {
	  if(! tau_trk->flagWithMask( (1<<xAOD::TauJetParameters::TauTrackFlag::coreTrack) | (1<<xAOD::TauJetParameters::TauTrackFlag::passTrkSelector))) continue; //originally it was coreTrack&passTrkSelector
	  if( (*track_it) == tau_trk->track()) alreadyUsed = true;
	}
	//if this track has already been used by another tau, don't associate it to this new one                                                               
	if(alreadyUsed) ATH_MSG_INFO( "Found Already Used track new, now removing: " << *track_it );
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
    tauTrackCon.push_back(track);

    ElementLink<xAOD::TrackParticleContainer> linkToTrackParticle;
    linkToTrackParticle.toContainedElement(*trackParticleCont, trackParticle);
    track->addTrackLink(linkToTrackParticle);

    track->setP4(trackParticle->pt(), trackParticle->eta(), trackParticle->phi(), trackParticle->m());
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::coreTrack, true);
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelector, true);
    // in case TrackClassifier is not run, still get sensible results                                                                                        
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::classifiedCharged, true);
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::unclassified, true);
    
    ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
    linkToTauTrack.toContainedElement(tauTrackCon, track);
    pTau.addTauTrackLink(linkToTauTrack);

    ATH_MSG_VERBOSE(name()     << " added core track nr: " << i
		    << " eta " << pTau.track(i)->eta()
		    << " phi " << pTau.track(i)->phi()
		    );
  }
  // set the charge, which is defined by the core tau tracks only                                                                                              
  pTau.setCharge(charge);

  for (unsigned int i = 0; i < wideTracks.size(); ++i) {
    const xAOD::TrackParticle* trackParticle = wideTracks.at(i);

    ATH_MSG_VERBOSE(name()     << " adding wide track nr: " << i
		    << " eta " << trackParticle->eta()
		    << " phi " << trackParticle->phi()
		    );

    xAOD::TauTrack* track = new xAOD::TauTrack();
    tauTrackCon.push_back(track);

    ElementLink<xAOD::TrackParticleContainer> linkToTrackParticle;
    linkToTrackParticle.toContainedElement(*trackParticleCont, trackParticle);
    track->addTrackLink(linkToTrackParticle);

    track->setP4(trackParticle->pt(), trackParticle->eta(), trackParticle->phi(), trackParticle->m());
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::wideTrack, true);
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::passTrkSelector, true);
    // in case TrackClassifier is not run, still get sensible results                                                                                        
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::classifiedIsolation, true); // for sake of trigger, reset in TauTrackClassifier
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::modifiedIsolationTrack, true); // for sake of trigger, reset in TauTrackClassifier
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::unclassified, true);

    ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
    linkToTauTrack.toContainedElement(tauTrackCon, track);
    pTau.addTauTrackLink(linkToTauTrack);

  }

  //These are set again in TauTrackClassifier                                                                                                                  
  pTau.setDetail(xAOD::TauJetParameters::nChargedTracks, (int) pTau.nTracks());
  pTau.setDetail(xAOD::TauJetParameters::nIsolatedTracks, (int) pTau.nTracks(xAOD::TauJetParameters::classifiedIsolation));

  for (unsigned int i = 0; i < otherTracks.size(); ++i) {
    const xAOD::TrackParticle* trackParticle = otherTracks.at(i);

    ATH_MSG_VERBOSE(name()     << " adding other track nr: " << i
		    << " eta " << trackParticle->eta()
		    << " phi " << trackParticle->phi()
		    );

    xAOD::TauTrack* track = new xAOD::TauTrack();
    tauTrackCon.push_back(track);

    ElementLink<xAOD::TrackParticleContainer> linkToTrackParticle;
    linkToTrackParticle.toContainedElement(*trackParticleCont, trackParticle);
    track->addTrackLink(linkToTrackParticle);

    track->setP4(trackParticle->pt(), trackParticle->eta(), trackParticle->phi(), trackParticle->m());
    float dR = track->p4().DeltaR(pTau.p4());
    if(dR<=0.2) track->setFlag(xAOD::TauJetParameters::TauTrackFlag::coreTrack, true);
    else track->setFlag(xAOD::TauJetParameters::TauTrackFlag::wideTrack, true);
    track->setFlag(xAOD::TauJetParameters::TauTrackFlag::unclassified, true);

    ElementLink<xAOD::TauTrackContainer> linkToTauTrack;
    linkToTauTrack.toContainedElement(tauTrackCon, track);
    pTau.addTauTrackLink(linkToTauTrack);

  }

  ATH_MSG_DEBUG("numTrack: " << "/" << pTau.nTracks());
  ATH_MSG_DEBUG("charge: " << "/" << pTau.charge());

  // extrapolate core tracks to calorimeter surface
  // store information only in ExtraDetailsContainer
  if(!m_bypassExtrapolator)
    {
      StatusCode sc;
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
        const xAOD::Vertex* primaryVertex) const
{
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
        std::vector<const xAOD::TrackParticle*> &otherTracks) const
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
StatusCode TauTrackFinder::extrapolateToCaloSurface(xAOD::TauJet& pTau) const {

    Trk::TrackParametersIdHelper parsIdHelper;

    int trackIndex = -1;
    const Trk::CaloExtension * caloExtension = nullptr;
    std::unique_ptr<Trk::CaloExtension> uniqueExtension ;
    for( xAOD::TauTrack* tauTrack : pTau.allTracks() ) {
        const xAOD::TrackParticle *orgTrack = tauTrack->track();
        trackIndex = orgTrack->index();

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

        if(!m_ParticleCacheKey.key().empty()){
          /*get the CaloExtension object*/
          ATH_MSG_VERBOSE("Using the CaloExtensionBuilder Cache");
          SG::ReadHandle<CaloExtensionCollection>  particleCache {m_ParticleCacheKey};
          caloExtension = (*particleCache)[trackIndex];
          ATH_MSG_VERBOSE("Getting element " << trackIndex << " from the particleCache");
          if( not caloExtension ){
            ATH_MSG_VERBOSE("Cache does not contain a calo extension -> Calculating with the a CaloExtensionTool" );
            uniqueExtension = m_caloExtensionTool->caloExtension(*orgTrack);
            caloExtension = uniqueExtension.get();
          }
        }else{
          /* If CaloExtensionBuilder is unavailable, use the calo extension tool */
          ATH_MSG_VERBOSE("Using the CaloExtensionTool");
          uniqueExtension = m_caloExtensionTool->caloExtension(*orgTrack);
          caloExtension = uniqueExtension.get();
        }

        if (!caloExtension)
        { 
            ATH_MSG_DEBUG("Track extrapolation failed");
        }
        else {
            const std::vector<const Trk::CurvilinearParameters*>& clParametersVector = caloExtension->caloLayerIntersections();
            if (clParametersVector.empty()) {
              ATH_MSG_DEBUG("Track extrapolation failed");
            }

            ATH_MSG_DEBUG("Scanning samplings");
            bool validECal = false;
            bool validHCal = false;
            for( const Trk::CurvilinearParameters * cur : clParametersVector ){
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
            if( not validECal and std::abs(orgTrack->pt()) < 2.48 ){
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
    }

    return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void TauTrackFinder::removeOffsideTracksWrtLeadTrk(std::vector<const xAOD::TrackParticle*> &tauTracks,
                           std::vector<const xAOD::TrackParticle*> &wideTracks,
                           std::vector<const xAOD::TrackParticle*> &otherTracks,
                           const xAOD::Vertex* tauOrigin,
                           double maxDeltaZ0) const
{
    float MAX=1e5;

    // need at least one core track to have a leading trk to compare with
    if (tauTracks.size()<1) return;

    // get lead trk parameters
    const xAOD::TrackParticle *leadTrack = tauTracks.at(0);
    float z0_leadTrk = getZ0(leadTrack, tauOrigin);

    if (z0_leadTrk > MAX-1) return; // bad lead trk -> do nothing

    ATH_MSG_VERBOSE("before z0 cut: #coreTracks=" << tauTracks.size() << ", #wideTracks=" << wideTracks.size() << ", #otherTracks=" << otherTracks.size());

    std::vector<const xAOD::TrackParticle*>::iterator itr;
    
    // skip leading track, because it is the reference
    itr = tauTracks.begin()+1;
    while (itr!=tauTracks.end()) {
        float z0 = getZ0(*itr, tauOrigin);
        float deltaZ0=z0 - z0_leadTrk;
        ATH_MSG_VERBOSE("core Trks: deltaZ0= " << deltaZ0);

        if ( std::abs(deltaZ0) < maxDeltaZ0 ) {++itr;}
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

        if ( std::abs(deltaZ0) < maxDeltaZ0 ) { ++itr; }
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
float TauTrackFinder::getZ0(const xAOD::TrackParticle* track, const xAOD::Vertex* vertex) const
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
#endif
