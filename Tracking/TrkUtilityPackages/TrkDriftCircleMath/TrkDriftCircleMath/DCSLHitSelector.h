/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    DCSLHitSelector() { m_selection.reserve(100); }

    const HitSelection& selectHitsOnTrack( const DCOnTrackVec& dcs ) const; 

    mutable HitSelection m_selection;
  };

  inline const DCSLHitSelector::HitSelection& DCSLHitSelector::selectHitsOnTrack( const DCOnTrackVec& dcs ) const
  {
    m_selection.clear();
    m_selection.resize(dcs.size(),1);
    for( unsigned int i=0;i<dcs.size();++i) {
      if( dcs[i].state() == DCOnTrack::OnTrack ) m_selection[i] = 0;
    }
    return m_selection; 
  }
  

}


#endif
