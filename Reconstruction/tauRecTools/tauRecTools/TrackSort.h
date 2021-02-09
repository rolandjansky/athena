/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_TRACKSORT_H
#define TAURECTOOLS_TRACKSORT_H

#ifndef XAOD_STANDALONE
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#endif
#include "AthLinks/ElementLink.h"
#include "xAODTracking/TrackParticle.h"

/**
 * @brief Helper method to sort tracks
 * 
 * Usage:
 * sort(track_begin, track_end, tauTrackSort);
 * We want pt0 > pt1 > ..., so the test on ptInvVert is < .
 * 
 */

class TrackSort {

public:
  TrackSort(){};

#ifndef XAOD_STANDALONE
  bool operator() (const ElementLink<Rec::TrackParticleContainer>& t1, const ElementLink<Rec::TrackParticleContainer> &t2) const
  {
    return fabs( (*t1)->pt() ) > fabs( (*t2)->pt() );
  };

  bool operator() ( const Rec::TrackParticle *t1, const Rec::TrackParticle *t2 ) const
  {
    return fabs( t1->pt() ) > fabs( t2->pt() );
  };
#endif

  bool operator() ( const xAOD::TrackParticle *t1, const xAOD::TrackParticle *t2 ) const
  {
    return fabs( t1->pt() ) > fabs( t2->pt() );
  };

};

#endif // TAURECTOOLS_TRACKSORT_H
