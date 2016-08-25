/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexSeedFinderUtils/PointOnTrack.h"

namespace Trk {

  PointOnTrack::PointOnTrack() : std::pair<const Perigee*,double>(0,0) {}

  PointOnTrack::PointOnTrack(const Perigee & first,double second): 
    std::pair<const Perigee*,double>(&first,second) {}

  PointOnTrack::PointOnTrack(const PointOnTrack & same) : 
    std::pair<const Perigee*,double>(same.first,same.second) {}

  PointOnTrack::PointOnTrack(const Perigee & perigee): 
    std::pair<const Perigee*,double>(&perigee,perigee.parameters()[Trk::phi]) {}

  const Perigee & PointOnTrack::getPerigee() const {
    return *(this->first);
  }

  double PointOnTrack::getPhiPoint() const {
    return this->second;
  }

  void PointOnTrack::setPhiPoint(double phi) {
    this->second=phi;
  }

  void PointOnTrack::setPerigee(const Perigee & perigee) {
    this->first=&perigee;
  }

}
