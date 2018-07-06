/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BtrigUtils.h"

#include <math.h>
#include <vector>

#include "Particle/TrackParticle.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"

#include "TrigSteeringEvent/Enums.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"

#include <exception>


//----------------------------------------------
// pT calculation using Trk::Track.
//----------------------------------------------

double PtVal(const std::vector<const Trk::Track*> & tracks) {

  std::vector<const Trk::Track*>::const_iterator trkItr    = tracks.begin();
  std::vector<const Trk::Track*>::const_iterator trkItrEnd = tracks.end();

  double px_sum = 0;
  double py_sum = 0;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    px_sum += fabs((*trkItr)->perigeeParameters()->pT()) * cos((*trkItr)->perigeeParameters()->parameters()[Trk::phi0]);
    py_sum += fabs((*trkItr)->perigeeParameters()->pT()) * sin((*trkItr)->perigeeParameters()->parameters()[Trk::phi0]);
  }

  double pt2 = px_sum*px_sum + py_sum*py_sum;

  if ( pt2 < 0 ) return 0;
  else          return sqrt(pt2);
}

//----------------------------------------------
// pT calculation using xAOD::TrackParticle.
//----------------------------------------------

double PtVal(const std::vector<const xAOD::TrackParticle*> & tracks) {

  std::vector<const xAOD::TrackParticle*>::const_iterator trkItr    = tracks.begin();
  std::vector<const xAOD::TrackParticle*>::const_iterator trkItrEnd = tracks.end();

  double px_sum = 0;
  double py_sum = 0;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    px_sum += fabs((*trkItr)->pt()) * cos((*trkItr)->phi());
    py_sum += fabs((*trkItr)->pt()) * sin((*trkItr)->phi());
  }

  double pt2 = px_sum*px_sum + py_sum*py_sum;

  if ( pt2 < 0 ) return 0;
  else          return sqrt(pt2);
}

//------------------------------------------------------------
// L_xy calculation using Trk::Track and two vertex positions.
//------------------------------------------------------------

double LxyVal(const double px, const double py, const Amg::Vector3D Vertex0, const Amg::Vector3D Vertex1) {

  double rdx = Vertex1[0] - Vertex0[0];
  double rdy = Vertex1[1] - Vertex0[1];

  double pt  = sqrt(px*px+py*py);
  double rxy = sqrt(rdx*rdx+rdy*rdy);

  double cosxy = 0.;
  if ( rxy != 0. ) cosxy = (px*rdx+py*rdy)/rxy/pt;

  return rxy*cosxy;
}

//--------------------------------------------------------------------
// Base invariant mass calculation using px,py,pz and mass-hypothesis.
//--------------------------------------------------------------------

double InvMass(const std::vector<double> & px, const std::vector<double> & py,
               const std::vector<double> & pz, const std::vector<double> & massHypo) {

  double px_sum = 0;
  double py_sum = 0;
  double pz_sum = 0;
  double e_sum  = 0;

  for ( unsigned int i=0; i<massHypo.size(); i++ ) {
    px_sum += px[i];
    py_sum += py[i];
    pz_sum += pz[i];
    e_sum  += sqrt(massHypo[i]*massHypo[i] + px[i]*px[i] + py[i]*py[i] + pz[i]*pz[i]);
  }

  double m2 = e_sum*e_sum - px_sum*px_sum - py_sum*py_sum - pz_sum*pz_sum;

  if ( m2 < 0 ) return 0;
  else          return sqrt(m2);
}



//--------------------------------------------------
// Invariant mass calculations using TrigInDetTrack.
//--------------------------------------------------

double InvMass(const std::vector<const TrigInDetTrack*> &tracks, const std::vector<double> & massHypo) {

  std::vector<const TrigInDetTrack*>::const_iterator trkItr    = tracks.begin();
  std::vector<const TrigInDetTrack*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    double theta = 2. * atan(exp(-1.0*((*trkItr)->param()->eta())));
    px.push_back( fabs((*trkItr)->param()->pT()) * cos((*trkItr)->param()->phi0()) );
    py.push_back( fabs((*trkItr)->param()->pT()) * sin((*trkItr)->param()->phi0()) );
    pz.push_back( fabs((*trkItr)->param()->pT()) / tan(theta) );
  }

  return InvMass(px, py, pz, massHypo);
}



//--------------------------------------------------------
// Invariant mass calculations using TrigInDetTrackFitPar.
//--------------------------------------------------------

double InvMass(const std::vector<const TrigInDetTrackFitPar*> & tracks, const std::vector<double> & massHypo) {

  std::vector<const TrigInDetTrackFitPar*>::const_iterator trkItr    = tracks.begin();
  std::vector<const TrigInDetTrackFitPar*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    double theta = 2. * atan(exp(-1.0*((*trkItr)->eta())));
    px.push_back( fabs((*trkItr)->pT()) * cos((*trkItr)->phi0()) );
    py.push_back( fabs((*trkItr)->pT()) * sin((*trkItr)->phi0()) );
    pz.push_back( fabs((*trkItr)->pT()) / tan(theta) );
  }

  return InvMass(px, py, pz, massHypo);
}



//--------------------------------------------------------
// Invariant mass calculations using Trk::MeasuredPerigee.
//--------------------------------------------------------

double InvMass(const std::vector<const Trk::TrackParameters*> & tracks, const std::vector<double> & massHypo) {

  std::vector<const Trk::TrackParameters*>::const_iterator trkItr    = tracks.begin();
  std::vector<const Trk::TrackParameters*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
    px.push_back( (*trkItr)->momentum()[Trk::px] );
    py.push_back( (*trkItr)->momentum()[Trk::py] );
    pz.push_back( (*trkItr)->momentum()[Trk::pz] );
  }

  return InvMass(px, py, pz, massHypo);
}

//--------------------------------------------------------
// Invariant mass calculations using Trk::Perigee.
//--------------------------------------------------------

double InvMass(const std::vector<const Trk::Perigee*> & tracks, const std::vector<double> & massHypo) {
    auto trkItr    = tracks.begin();
    auto trkItrEnd = tracks.end();

    std::vector<double> px, py, pz;
    
    for ( ; trkItr!=trkItrEnd; trkItr++ ) {
        px.push_back( (*trkItr)->momentum()[Trk::px] );
        py.push_back( (*trkItr)->momentum()[Trk::py] );
        pz.push_back( (*trkItr)->momentum()[Trk::pz] );
    }
    
    return InvMass(px, py, pz, massHypo);
}


//----------------------------------------------
// Invariant mass calculations using Trk::Track.
//----------------------------------------------

double InvMass(const std::vector<const Trk::Track*> & tracks, const std::vector<double> & massHypo) {

  std::vector<const Trk::Track*>::const_iterator trkItr    = tracks.begin();
  std::vector<const Trk::Track*>::const_iterator trkItrEnd = tracks.end();

  std::vector<double> px, py, pz;

  for ( ; trkItr!=trkItrEnd; trkItr++ ) {
      // JW EDM
      //    px.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * ((*trkItr)->perigeeParameters()->cosPhi()) );
      //    py.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * ((*trkItr)->perigeeParameters()->sinPhi()) );
      //    pz.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * ((*trkItr)->perigeeParameters()->cotTheta()) );
      px.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * cos((*trkItr)->perigeeParameters()->parameters()[Trk::phi0]));
      py.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * sin((*trkItr)->perigeeParameters()->parameters()[Trk::phi0]));
      pz.push_back( fabs((*trkItr)->perigeeParameters()->pT()) * 1./tan((*trkItr)->perigeeParameters()->parameters()[Trk::theta]));

  }

  return InvMass(px, py, pz, massHypo);
}

double fabsDeltaPhi(double phi1, double phi2) {
    double dPhi=fabs(phi1 - phi2);
    if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
    return dPhi;
}

double fabsDeltaEta(double eta1, double eta2) {
    return fabs( eta1 - eta2);
}

