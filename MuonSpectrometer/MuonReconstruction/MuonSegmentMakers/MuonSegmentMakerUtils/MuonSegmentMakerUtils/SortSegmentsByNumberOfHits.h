/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SORTSEGMENTSBYNUMBEROFHITS_H
#define SORTSEGMENTSBYNUMBEROFHITS_H

#include "MuonSegment/MuonSegment.h"
#include "TrkEventPrimitives/FitQuality.h"

namespace Muon {

  class SortSegmentsByNumberOfHits {
  public:
    bool operator()( const std::unique_ptr<MuonSegment>& seg1, const std::unique_ptr<MuonSegment>& seg2){

      unsigned int nhits1 = seg1->containedMeasurements().size();
      unsigned int nhits2 = seg2->containedMeasurements().size();
      if( nhits1 > nhits2 ) return true;
      if( nhits1 < nhits2 ) return false;

      // prefer segments with fit quality (always expected)
      const Trk::FitQuality* fq1 = seg1->fitQuality();
      const Trk::FitQuality* fq2 = seg2->fitQuality();
      if( !fq1 && fq2 ) return false;
      if( fq1  && !fq2 ) return true;
      if( !fq1 && !fq2 ) {
        return true;
      }

      if( fq1->numberDoF() > fq2->numberDoF() ) return true;
      if( fq1->numberDoF() < fq2->numberDoF() ) return false;

      // select candidate with smallest chi2
      double chi2Ndof1 = fq1->chiSquared()/fq1->numberDoF();
      double chi2Ndof2 = fq2->chiSquared()/fq2->numberDoF();
      return chi2Ndof1 < chi2Ndof2;
    }
  };

}

#endif
