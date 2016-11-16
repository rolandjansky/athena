/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTau/TauxAODHelpers.h"

typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;

// ==================================================================
const xAOD::TruthParticle* xAOD::TauHelpers::getTruthParticle(const xAOD::IParticle* particle, bool debug)
{
  return getLink<xAOD::TruthParticle>(particle, "truthParticleLink", debug);
}

// ==================================================================
std::vector< ElementLink< xAOD::TrackParticleContainer > > xAOD::TauHelpers::allTrackParticleLinks(const xAOD::TauJet* tau){
  xAOD::TauTrack::TrackFlagType mask=0;
  return trackParticleLinksWithMask(tau, mask);
}

// ==================================================================
std::vector< ElementLink< xAOD::TrackParticleContainer > > xAOD::TauHelpers::trackParticleLinks(const xAOD::TauJet* tau, xAOD::TauJetParameters::TauTrackFlag flag/*=xAOD::TauJetParameters::TauTrackFlag::classifiedCharged*/){
  xAOD::TauTrack::TrackFlagType mask=1<<flag;
  return trackParticleLinksWithMask(tau, mask);
}

// ==================================================================
std::vector< ElementLink< xAOD::TrackParticleContainer > > xAOD::TauHelpers::trackParticleLinksWithMask(const xAOD::TauJet* tau, xAOD::TauTrack::TrackFlagType mask){
  std::vector< ElementLink< xAOD::TrackParticleContainer > > links;
  std::vector< const xAOD::TauTrack* > allTracks = tau->allTracks();
  for ( const xAOD::TauTrack* trk : allTracks ){
    if(trk->trackLinks().size()<1) continue;
    if(trk->flagWithMask(mask)) 
      links.push_back(trk->trackLinks()[0]);    
  }
  return links;
}
    

// ==================================================================
xAOD::TauTrack* xAOD::TauHelpers::tauTrackNonConst( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, int index, xAOD::TauJetParameters::TauTrackFlag flag/*=xAOD::TauJetParameters::TauTrackFlag::classifiedCharged */){
  xAOD::TauTrack::TrackFlagType mask=1<<flag;
  return tauTrackNonConstWithMask(tau, trackCont, index, mask);
  
}

// ==================================================================
xAOD::TauTrack* xAOD::TauHelpers::tauTrackNonConstWithMask( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, int index,  xAOD::TauTrack::TrackFlagType mask ){
  int accepted_track=0;
  for( ElementLink< xAOD::TauTrackContainer > link : tau->allTauTrackLinks() ){
    const xAOD::TauTrack* c_trk(0);
    if(link.isValid()) c_trk = *link;
    if(c_trk==0) continue;
    if(c_trk->flagWithMask(mask)){
      
      if(accepted_track==index) {
	if(dynamic_cast<const xAOD::TauTrackContainer*> (link.getStorableObjectPointer())!=trackCont){
	  std::cerr << "TauxAODHelpers::tauTrackNonConstWithMask, inconsistent containers given" << std::endl;
	  return 0;
	}
	return trackCont->at(link.index());
      }
      accepted_track++;
    }
  }
  return 0;
}

// ==================================================================
std::vector<xAOD::TauTrack*> xAOD::TauHelpers::tauTracksNonConst( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, xAOD::TauJetParameters::TauTrackFlag flag/*=xAOD::TauJetParameters::TauTrackFlag::classifiedCharged*/ ){
  xAOD::TauTrack::TrackFlagType mask=1<<flag;
  return tauTracksNonConstWithMask(tau, trackCont, mask);
}

// ==================================================================
std::vector<xAOD::TauTrack*> xAOD::TauHelpers::tauTracksNonConstWithMask( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, xAOD::TauTrack::TrackFlagType mask ){
  std::vector<xAOD::TauTrack*> v;
  for( ElementLink< xAOD::TauTrackContainer > link : tau->allTauTrackLinks() ){
    if(link.isValid() && (*link)->flagWithMask(mask) && dynamic_cast<const xAOD::TauTrackContainer*> (link.getStorableObjectPointer()) == trackCont) v.push_back( trackCont->at(link.index()));
  }
  return v;
}

// ==================================================================
std::vector<xAOD::TauTrack*> xAOD::TauHelpers::allTauTracksNonConst( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont ){
  xAOD::TauTrack::TrackFlagType mask=0;
  return tauTracksNonConstWithMask(tau, trackCont, mask);
}
