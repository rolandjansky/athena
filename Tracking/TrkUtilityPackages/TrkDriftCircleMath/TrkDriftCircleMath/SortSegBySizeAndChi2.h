/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_SORTSEGBYSIZEANDCHI2_H
#define DCMATH_SORTSEGBYSIZEANDCHI2_H

#include "TrkDriftCircleMath/Segment.h"

namespace TrkDriftCircleMath {

  struct SortSegByNumberOfMDTHitsAndChi2 : public std::binary_function<class Segment, class Segment, bool> 
  {
    bool operator()(const Segment& seg1, const Segment& seg2 )
    {

      // prefer segment with T0 fit
      if( seg1.hasT0Shift() && !seg2.hasT0Shift() ) return true;
      else if( !seg1.hasT0Shift() && seg2.hasT0Shift() ) return false;

      // sort by numbers of hits on track
      if( seg1.hitsOnTrack() > seg2.hitsOnTrack() ) return true;
      else if( seg1.hitsOnTrack() < seg2.hitsOnTrack() ) return false;


      // take the one with less empty crossed tubes
      if( seg1.emptyTubes().size() < seg2.emptyTubes().size() ) return true;
      else if( seg1.emptyTubes().size() > seg2.emptyTubes().size() ) return false;

      // take one with associated clusters
      if( seg1.clusterLayers() >  0 && seg2.clusterLayers() == 0 ) return true;
      if( seg1.clusterLayers() == 0 && seg2.clusterLayers() >  0 ) return false;

      // if equal sort by number of deltas
      if( seg1.deltas() < seg2.deltas() ) return true;
      else if( seg1.deltas() > seg2.deltas() ) return false;


      // finally take the one with best chi2
      return seg1.chi2() < seg2.chi2();
    }

  };

  struct SortSegByNumberOfHitsAndChi2 : public std::binary_function<class Segment, class Segment, bool> 
  {
    bool operator()(const Segment& seg1, const Segment& seg2 )
    {
      // prefer segment with T0 fit
      if( seg1.hasT0Shift() && !seg2.hasT0Shift() ) return true;
      else if( !seg1.hasT0Shift() && seg2.hasT0Shift() ) return false;

      unsigned int nhits1 = seg1.hitsOnTrack() + seg1.clusterLayers();
      unsigned int nhits2 = seg2.hitsOnTrack() + seg2.clusterLayers();

      // sort by numbers of hits on track
      if( nhits1 > nhits2 ) return true;
      else if( nhits1 < nhits2 ) return false;

      // take the one with less empty crossed tubes
      if( seg1.emptyTubes().size() < seg2.emptyTubes().size() ) return true;
      else if( seg1.emptyTubes().size() > seg2.emptyTubes().size() ) return false;

      // finally take the one with best chi2
      return seg1.chi2() < seg2.chi2();
    }

  };

}

#endif
