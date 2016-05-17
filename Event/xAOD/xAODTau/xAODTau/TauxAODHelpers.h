// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauxAODHelpers.h 740338 2016-04-14 16:03:58Z griffith $
#ifndef XAOD_TAUXAODHELPERS_H
#define XAOD_TAUXAODHELPERS_H

//Truth Includes
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODTau/TauJet.h"
#include "xAODTau/TauTrackContainer.h"

#include <cstddef>
#include <set>
#include <vector>

namespace xAOD {

  namespace TauHelpers{
    
    // @brief Access to element link to object of type T stored in auxdata
    template<class T>
    const T* getLink(const xAOD::IParticle*, std::string, bool debug = false);
    
    ///@brief return the truthParticle associated to the given IParticle (if any)
    const xAOD::TruthParticle* getTruthParticle(const xAOD::IParticle*, bool debug = false);

    //means to bypass the tauTrack
    std::vector< ElementLink< xAOD::TrackParticleContainer > > allTrackParticleLinks(const xAOD::TauJet* tau);
    std::vector< ElementLink< xAOD::TrackParticleContainer > > trackParticleLinks(const xAOD::TauJet* tau, xAOD::TauJetParameters::TauTrackFlag flag=xAOD::TauJetParameters::TauTrackFlag::classifiedCharged);
    std::vector< ElementLink< xAOD::TrackParticleContainer > > trackParticleLinksWithMask(const xAOD::TauJet* tau, xAOD::TauTrack::TrackFlagType mask);
    
    //means to get a non-const TauTrack
    xAOD::TauTrack* tauTrackNonConst( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, int index, xAOD::TauJetParameters::TauTrackFlag flag=xAOD::TauJetParameters::TauTrackFlag::classifiedCharged );
    xAOD::TauTrack* tauTrackNonConstWithMask( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, int index,  xAOD::TauTrack::TrackFlagType mask );
    std::vector<xAOD::TauTrack*> tauTracksNonConst( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, xAOD::TauJetParameters::TauTrackFlag flag=xAOD::TauJetParameters::TauTrackFlag::classifiedCharged );
    std::vector<xAOD::TauTrack*> tauTracksNonConstWithMask( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont, xAOD::TauTrack::TrackFlagType mask );
    std::vector<xAOD::TauTrack*> allTauTracksNonConst( const xAOD::TauJet* tau, xAOD::TauTrackContainer* trackCont );



  }// TauHelpers

} // namespace xAOD

#include "xAODTau/TauxAODHelpers.icc"
  
#endif // XAOD_TAUXAODHELPERS_H
