/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_DCSLFITTER_H
#define DCMATH_DCSLFITTER_H

#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/DCOnTrack.h"
#include "TrkDriftCircleMath/Line.h"

#include <vector>

namespace TrkDriftCircleMath {

  class DCSLFitter {
  public:
    typedef std::vector<int>         HitSelection;
    struct FitData {
      double y; // local y position
      double z; // local z position
      double r; // drift radius
      double w; // weight
      double rw; // weighted radius
      double ryw; // weighted y position
      double rzw; // weighted z position
    };
  public:
    DCSLFitter();

    virtual ~DCSLFitter() {}
    virtual bool fit( Segment& result, const Line& line, const DCOnTrackVec& dcs, double t0Seed = -99999. ) const;
    virtual bool fit( Segment& result, const Line& line, const DCOnTrackVec& dcs, const HitSelection& selection, double t0Seed = -99999. ) const;

    void debug(bool debug) { m_debug = debug; }

  protected:
    bool m_debug;  
  };

  inline
    bool DCSLFitter::fit( Segment& result, const Line& line, const DCOnTrackVec& dcs, double ) const 
  { 
    HitSelection selection(dcs.size(),0);
    return fit( result, line, dcs, selection ); 
  }
}


#endif
