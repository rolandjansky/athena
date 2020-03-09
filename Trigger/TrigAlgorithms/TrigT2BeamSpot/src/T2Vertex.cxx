/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2Vertex.cxx 793162 2017-01-20 03:48:25Z ssnyder $
//

// This class
#include "T2Vertex.h"

// Externals
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TMath.h"

#include <functional>
#include <numeric>
#include <cmath>
using Gaudi::Units::GeV;
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

  template <class T>
  struct TrkSumOf 
  {
    double operator()( double x, const Trk::Track* track ) { return x + T()( track ); }
  };


  struct TrkTrackPt 
  {
    double operator()( const Trk::Track* track ) {
      const Trk::TrackParameters* params = track->perigeeParameters();
      return std::abs(sin(params->parameters()[Trk::theta])/params->parameters()[Trk::qOverP])/GeV;
    }
  };


  struct TrkTrackPt2 
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
