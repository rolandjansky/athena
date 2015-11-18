/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2Track.cxx 702277 2015-10-22 10:33:51Z smh $
//

#include "T2Track.h"
#include "TMath.h"


namespace PESA
{
  int trackNDF( const TrigInDetTrack& track )
  {
    // To get the n.d.f. take the number of silicon measurements (2 x N SPs) and subtract 5 helix params
    const int trackNDF = ( track.NPixelSpacePoints() + track.NSCT_SpacePoints() ) * 2 - 5;
    return trackNDF;
  }

  double trackChi2Prob( const T2Track& track )
  {
    // FIXME: unify with vertexChi2Prob()
    double chi2Prob = 0.;
    const int    ndf  = track.NDF();
    const double chi2 = track.Qual() * track.NDF();
    if ( ndf  > 0  &&  chi2 > 0. && ! std::isinf( chi2 ) )
      {
        chi2Prob = TMath::Prob(chi2,ndf);
      }
    return chi2Prob;
  }


  std::ostream& operator<<( std::ostream& os, const T2Track& track )
  {
    os
     << "Track pT        = " << track.Pt      () << '\n'
     << "Track eta       = " << track.Eta     () << '\n'
     << "Track phi       = " << track.Phi     () << '\n'
     << "Track z0        = " << track.Z0      () << " +- " << track.Z0err() << '\n'
     << "Track d0        = " << track.D0      () << " +- " << track.D0err() << '\n'
     << "Track fit ndf   = " << track.NDF     () << '\n'
     << "Track chi2/ndf  = " << track.Qual    () << '\n'
     << "Track chi2 prob = " << track.Chi2Prob() << '\n'
     << "Track Si hits   = " << track.SiHits  () << '\n'
     << "Track Pixel hits= " << track.PIXHits () << '\n'
     << "Track SCT hits  = " << track.SCTHits () << '\n'
     << "Track TRT hits  = " << track.TRTHits () << '\n'
      ;
    return os;
  }

}
