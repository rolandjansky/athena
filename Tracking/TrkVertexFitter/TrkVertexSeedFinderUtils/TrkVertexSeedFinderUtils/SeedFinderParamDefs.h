/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_SEEDFINDERPARAMDEFS_H
#define TRKVERTEXSEEDFINDERUTILS_SEEDFINDERPARAMDEFS_H


#include <algorithm>
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkVertexSeedFinderUtils/PointOnTrack.h"
#include "TrkVertexSeedFinderUtils/TwoTracks.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

  typedef std::pair < Amg::Vector3D , double > PositionAndWeight;
  typedef std::pair<double,double> DoubleAndWeight;
  typedef std::pair<PointOnTrack,PointOnTrack> TwoPointOnTrack;
  typedef std::pair<Amg::Vector3D,Amg::Vector3D> TwoPoints;
//  typedef std::pair<Perigee*,Perigee*> TwoTracks;

}

#endif
