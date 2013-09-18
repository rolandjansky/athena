/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTMERGER_H
#define MUON_IMUONSEGMENTMERGER_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

static const InterfaceID IID_IMuonSegmentMerger("Muon::IMuonSegmentMerger",1,0);




namespace Muon {
  
  class MuonSegment;

  /**
     @brief tool to merge segment with overlapping hits in the precision plain but different phi hits

  */
  class IMuonSegmentMerger : virtual public IAlgTool {
  public:
    
    typedef std::vector<const MuonSegment*> SegVec;
    typedef SegVec::iterator                SegIt;
    typedef SegVec::const_iterator          SegCit;

  public:
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_IMuonSegmentMerger; }

     /** @brief find segments sharing all MDT hits but different phi hits The caller should NOT take ownership of the segments */
    virtual SegVec findDuplicates( const SegVec& segments ) const = 0;

    /** @brief merges duplicates in a vector of segments. The callers should take ownership of the new Segments.*/
    virtual const MuonSegment* merge( const SegVec& segments ) const = 0;

  };
}

#endif
