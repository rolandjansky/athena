/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2Track.cxx 702277 2015-10-22 10:33:51Z smh $
//

#include "T2Track.h"
#include "TMath.h"


namespace PESA
{

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
