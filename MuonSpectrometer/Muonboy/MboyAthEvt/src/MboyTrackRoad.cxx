/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyAthEvt/MboyTrackRoad.h"
#include <sstream>

MboyTrackRoad::MboyTrackRoad():Trk::TrackRoad(){}
MboyTrackRoad::MboyTrackRoad(const Amg::Vector3D& gpos , const Amg::Vector3D& gdir, double dEta, double dPhi, double wei)
  : Trk::TrackRoad(gpos, gdir, dEta, dPhi)
{
  m_wei = wei;
}

MboyTrackRoad::~MboyTrackRoad(){}

double
MboyTrackRoad::weight() const 
{
  return m_wei;
}

std::string 
MboyTrackRoad::toString() const
{
  std::ostringstream oss;
  oss << Trk::TrackRoad::toString() << "wei=" << m_wei;
  return oss.str();
}
