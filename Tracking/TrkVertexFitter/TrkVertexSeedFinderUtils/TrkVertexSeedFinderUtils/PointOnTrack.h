/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_POINTONTRACK_H
#define TRKVERTEXSEEDFINDERUTILS_POINTONTRACK_H

#include "TrkParameters/TrackParameters.h"

namespace Trk {
  
  class PointOnTrack : public std::pair<const Perigee*,double> 
  {
  public:
    PointOnTrack();
    PointOnTrack(const Perigee & first,double second);
    PointOnTrack(const PointOnTrack & same);
    PointOnTrack(const Perigee & perigee);
    ~PointOnTrack() {};
    const Perigee & getPerigee() const;
    double getPhiPoint() const;
    void setPhiPoint(double phi);
    void setPerigee(const Perigee & perigee);
  };

}

#endif
