/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_ISSUBSETSEGMENT_H
#define DCMATH_ISSUBSETSEGMENT_H

#include "TrkDriftCircleMath/Segment.h"
#include "TrkDriftCircleMath/ResolvedCollection.h"
#include "TrkDriftCircleMath/SharedHitsSegments.h"
/** Function object to check whether two Segments are sub/super sets 
    or different.  
 */


namespace TrkDriftCircleMath {

  template<class CDC>
    struct IsSubsetSegment {
      IsSubsetSegment() : m_phiCut(0.05), m_y0Cut(0.5) {}
      IsSubsetSegment(double phiCut, double y0Cut) : m_phiCut(phiCut), m_y0Cut(y0Cut) {}

      //SubsetState 
      int operator()( const Segment& seg1, const Segment& seg2 ) const {

	// check angular distance between segments and distance from origine
	// only segments that have similar angles and offsets are considered subsets
	if( std::abs( seg1.line().phi() - seg2.line().phi() ) < m_phiCut && 
	    std::abs( seg1.line().y0()  - seg2.line().y0()  ) < m_y0Cut ) {

	  
	  SharedHitsSegments sharedHitsSegments(true);
	  sharedHitsSegments(seg1,seg2);

	  unsigned int onlyInSeg1 = sharedHitsSegments.onlyInSeg1();
	  unsigned int onlyInSeg2 = sharedHitsSegments.onlyInSeg2();
/* 	  std::cout << " seg1 " << seg1 << " ---- seg1 " << onlyInSeg1 << std::endl << std::endl; */
/* 	  std::cout << " seg2 " << seg2 << " ---- seg2 " << onlyInSeg2 << std::endl << std::endl; */
	  // segment 1 contains no hits that are not in segment 2
	  if( onlyInSeg1 == 0 ){
	    //check is segment 1 is a single ml segment that should be kept for the curved segment finder
	    if(seg1.hitsMl1() == 0 || seg1.hitsMl2() == 0) {
	      if(seg2.hitsMl2() < 3 || seg2.hitsMl1() < 3) {
		return ResolvedCollection<SegVec,IsSubsetSegment>::Different;	   
	      }
	      if(seg2.hitsOutOfTime() > 0) {
		return ResolvedCollection<SegVec,IsSubsetSegment>::Different;	      
	      }
	    }
	    // segment 1 contains the same hits as segment 2
	    if( onlyInSeg2 == 0 ){
	      // chose segment with best chi2
	      if( seg1.chi2() < seg2.chi2() ) return ResolvedCollection<SegVec,IsSubsetSegment>::SuperSet;
	      else return ResolvedCollection<SegVec,IsSubsetSegment>::SubSet;
	    }
	    return ResolvedCollection<SegVec,IsSubsetSegment>::SubSet;
	  }else{
	    if( onlyInSeg2 == 0 ){
	      return ResolvedCollection<SegVec,IsSubsetSegment>::SuperSet;
	    }	
	  }

	}
	return ResolvedCollection<SegVec,IsSubsetSegment>::Different;
      }

      private:
      double m_phiCut;
      double m_y0Cut;
    };

}
#endif
