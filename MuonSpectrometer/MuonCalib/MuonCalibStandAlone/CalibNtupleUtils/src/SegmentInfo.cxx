/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleUtils/SegmentInfo.h"


namespace MuonCalib {

  double SegmentInfo::segQ()
  {
    double sq = chi2 + 5.1*(nHoos + emptyTubes + nDeltas);
    sq  /= (nHots-2);
    if( trigHitRegion && nTrigPhi + nTrigEta == 0) sq += 6.;
    return sq;
  }

  double SegmentInfo::segQ2()
  {
    double sq = segQ();
    int iseg = nGoodSegPat < 2 ? 1 : 0;
    sq += 10.*(dthetaPat/0.2) + 8*iseg;
    return sq;
  }

  std::ostream& SegmentInfo::print( std::ostream& os) const
    {
      os << " SegInfo: chi2 " << chi2 << " hot " << nHots << " hoo " << nHoos << " delta " << nDeltas
	 << " empty " << emptyTubes << " theta " << theta << " invcurv " << invcurvature;
      if( trigHitRegion ) os << " ntrPhi " << nTrigPhi << " ntrEta " << nTrigEta;
      else if( phiPat ) os << " no trig, but phi ";
      else os << " no trigger, no phi";
      return os;
    }

}


std::ostream& operator<<( std::ostream& os, const MuonCalib::SegmentInfo& info) 
{
  info.print(os);
  return os;
}
