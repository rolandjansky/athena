/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_DCSLHITSELECTOR_H
#define DCMATH_DCSLHITSELECTOR_H

#include "TrkDriftCircleMath/DCSLFitter.h"
#include "TrkDriftCircleMath/DCOnTrack.h"


namespace TrkDriftCircleMath {

  class DCSLHitSelector {
  public:
    typedef DCSLFitter::HitSelection HitSelection;

  public:
    DCSLHitSelector() {}

    const HitSelection selectHitsOnTrack( const DCOnTrackVec& dcs ) const; 

  };

  inline const DCSLHitSelector::HitSelection DCSLHitSelector::selectHitsOnTrack( const DCOnTrackVec& dcs ) const
  {
    HitSelection selection;
    selection.resize(dcs.size(),1);
    for( unsigned int i=0;i<dcs.size();++i) {
      if( dcs[i].state() == DCOnTrack::OnTrack ) selection[i] = 0;
    }
    return selection; 
  }
  

}


#endif
