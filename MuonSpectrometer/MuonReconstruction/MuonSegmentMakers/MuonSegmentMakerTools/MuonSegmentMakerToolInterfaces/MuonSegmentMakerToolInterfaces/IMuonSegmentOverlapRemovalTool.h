/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTOVERLAPREMOVALTOOL_H
#define MUON_IMUONSEGMENTOVERLAPREMOVALTOOL_H

#include "GaudiKernel/IAlgTool.h"


#include <vector>

static const InterfaceID IID_IMuonSegmentOverlapRemovalTool("Muon::IMuonSegmentOverlapRemovalTool",1,0);



namespace Muon {
  
  class MuonSegment;
  class MuonSegmentCombination;

  /**
     @brief abstract interface for tools removing duplicates from segment collections and segment combinations

  */
  class IMuonSegmentOverlapRemovalTool : virtual public IAlgTool {
  public:
    typedef std::vector<const MuonSegment*> SegVec;
    typedef SegVec::iterator                SegIt;
    typedef SegVec::const_iterator          SegCit;
  public:
    
    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_IMuonSegmentOverlapRemovalTool; }

    /** @brief remove duplicates from a vector of segments. The caller should take 
	ownership of the segments */
    virtual SegVec removeDuplicates( const SegVec& segments ) const = 0;

    /** @brief remove duplicates from a MuonSegmentCombination.
        The callers should take ownership of the new SegmentCombination. If the combination is 
        unchanged the routine will return a pointer to the input combination. The caller should
        check whether the two pointers are identical and avoid a double delete */
    virtual const MuonSegmentCombination* removeDuplicates( const MuonSegmentCombination& combi ) const = 0;
  };

}

#endif
