/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2Vertex.cxx 702277 2015-10-22 10:33:51Z smh $
//

// This class
#include "T2Vertex.h"

// Externals
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TMath.h"
using Gaudi::Units::GeV;

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
        chi2Prob = TMath::Prob(chi2,ndf);
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

  template <class T>
  struct TrkSumOf : public binary_function< double, Trk::Track, double >
  {
    double operator()( double x, const Trk::Track* track ) { return x + T()( track ); }
  };


  double vertexSumPt( const TrackInVertexList& tracks )
  {
    return accumulate( tracks.begin(), tracks.end(), 0., SumOf< TrackPt >() );
  }


  double vertexSumPt2( const TrackInVertexList& tracks )
  {
    return sqrt( accumulate( tracks.begin(), tracks.end(), 0., SumOf< TrackPt2 >() ) );
  }

  struct TrkTrackPt : public unary_function< Trk::Track, double >
  {
    double operator()( const Trk::Track* track ) {
      const Trk::TrackParameters* params = track->perigeeParameters();
      return std::abs(sin(params->parameters()[Trk::theta])/params->parameters()[Trk::qOverP])/GeV;
    }
  };


  struct TrkTrackPt2 : public unary_function< Trk::Track, double >
  {
    double operator()( const Trk::Track* track ) { const double pT = TrkTrackPt()( track ); return pT*pT; }
  };



  double vertexSumPt( const TrackCollection& tracks )
  {
    return accumulate( tracks.begin(), tracks.end(), 0., TrkSumOf< TrkTrackPt >() );
  }


  double vertexSumPt2( const TrackCollection& tracks )
  {
    return sqrt( accumulate( tracks.begin(), tracks.end(), 0., TrkSumOf< TrkTrackPt2 >() ) );
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
