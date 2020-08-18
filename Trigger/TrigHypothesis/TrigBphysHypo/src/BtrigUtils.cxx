/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BtrigUtils.h"

#include <math.h>

//----------------------------------------------
// pT calculation using xAOD::TrackParticle.
//----------------------------------------------

double PtVal(const std::vector<const xAOD::TrackParticle*> & tracks) {

  std::vector<const xAOD::TrackParticle*>::const_iterator trkItr    = tracks.begin();
  std::vector<const xAOD::TrackParticle*>::const_iterator trkItrEnd = tracks.end();

  double px_sum = 0;
  double py_sum = 0;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    px_sum += std::abs((*trkItr)->pt()) * cos((*trkItr)->phi());
    py_sum += std::abs((*trkItr)->pt()) * sin((*trkItr)->phi());
  }

  double pt2 = px_sum*px_sum + py_sum*py_sum;

  if ( pt2 < 0 ) return 0;
  else          return sqrt(pt2);
}

//------------------------------------------------------------
// L_xy calculation using Trk::Track and two vertex positions.
//------------------------------------------------------------

double LxyVal(const double px, const double py, const Amg::Vector3D &Vertex0, const Amg::Vector3D &Vertex1) {

  double rdx = Vertex1[0] - Vertex0[0];
  double rdy = Vertex1[1] - Vertex0[1];

  double pt  = std::sqrt(px*px+py*py);
  double rxy = std::sqrt(rdx*rdx+rdy*rdy);

  double cosxy = 0.;
  if ( rxy != 0. ) cosxy = (px*rdx+py*rdy)/rxy/pt;

  return rxy*cosxy;
}

