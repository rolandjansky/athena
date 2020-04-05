/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGINDETTRACKFITTER_H__ 
#define __ITRIGINDETTRACKFITTER_H__ 

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrkTrack/TrackCollection.h"

static const InterfaceID IID_ITrigInDetTrackFitter("ITrigInDetTrackFitter", 1 , 0); 

class ITrigInDetTrackFitter: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigInDetTrackFitter;
  }
  virtual void fit(const TrackCollection&, TrackCollection&, const Trk::ParticleHypothesis& matEffects) const = 0;
};

#endif


