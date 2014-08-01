/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2Vertex.cxx 527425 2012-11-23 14:23:00Z bartoldu $
//

// This class
#include "T2Vertex.h"

// Externals
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::GeV;

#include <functional>
#include <numeric>
using std::unary_function;
using std::binary_function;
using std::accumulate;


namespace PESA
{
  double vertexChi2Prob( const T2Vertex& vertex )
  {
    // FIXME: unify with trackChi2Prob()
    double chi2Prob = 0.;
    const int    ndf  = vertex.NDF();
    const double chi2 = vertex.Qual() * vertex.NDF();
    if ( ndf  > 0  &&  chi2 > 0. && ! std::isinf( chi2 ) )
      {
        chi2Prob = 1.0 - Genfun::CumulativeChiSquare( ndf )( chi2 );
      }
    return chi2Prob;
  }


  double tiltedBeamPositionAtZPoint( double Zref, double nominalZPosition,
                                     double nominalTransversePosition, double tilt )
  {
    /// Calculate the tilted beam position at Z != 0
    return nominalTransversePosition + tan(tilt) * (Zref-nominalZPosition);
  }


  struct TrackPt : public unary_function< TrigInDetTrack, double >
  {
    double operator()( const TrigInDetTrack* track ) { return abs(track->param()->pT())/GeV; }
  };


  struct TrackPt2 : public unary_function< TrigInDetTrack, double >
  {
    double operator()( const TrigInDetTrack* track ) { const double pT = TrackPt()( track ); return pT*pT; }
  };


  template <class T>
  struct SumOf : public binary_function< double, TrigInDetTrack, double >
  {
    double operator()( double x, const TrigInDetTrack* track ) { return x + T()( track ); }
  };


  double vertexSumPt( const TrackInVertexList& tracks )
  {
    return accumulate( tracks.begin(), tracks.end(), 0., SumOf< TrackPt >() );
  }


  double vertexSumPt2( const TrackInVertexList& tracks )
  {
    return sqrt( accumulate( tracks.begin(), tracks.end(), 0., SumOf< TrackPt2 >() ) );
  }

//    double sumPt = 0.;
//
//    for ( TrackInVertexList::const_iterator track = tracks.begin(); track != tracks.end(); ++track )
//      {
//        const double pt = track->abs(track.param()->pT())/GeV;
//        sumPt += pT;
//      }


  std::ostream& operator<<( std::ostream& os, const T2Vertex& vertex )
  {
    os
      << "Vertex x +/- dx = " << vertex.X() << " +/- " << vertex.Xerr() << '\n'
      << "Vertex y +/- dy = " << vertex.Y() << " +/- " << vertex.Yerr() << '\n'
      << "Vertex z +/- dz = " << vertex.Z() << " +/- " << vertex.Zerr() << '\n'
      << "Vertex N tracks = " << vertex.NTrks()    << '\n'
      << "Vertex fit mass = " << vertex.Mass()     << '\n'
      << "Vertex fit qual = " << vertex.Qual()     << '\n'
      << "Vertex fit prob = " << vertex.Chi2Prob() << '\n'
      ;
    return os;
  }

}
