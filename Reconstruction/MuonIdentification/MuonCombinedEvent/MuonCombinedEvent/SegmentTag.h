/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_SEGMENTTAG_H
#define MUONCOMBINEDEVENT_SEGMENTTAG_H


#include "MuonCombinedEvent/TagBase.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"


namespace MuonCombined {
  
  /** TagBase implementation for a segment tagger */
  class SegmentTag : public TagBase {
  public:
    
    /** Constructor taking a match chi2 as input
    */
    SegmentTag( const std::vector< MuonCombined::MuonSegmentInfo >& segmentsInfo );

    /** destructor */
    ~SegmentTag();

    /** access */
    //  double matchChi2() const;
    const std::vector< MuonSegmentInfo >& segmentsInfo() const;

    /** access to associated segments, empty vector if non available */
    std::vector<const Muon::MuonSegment*> associatedSegments() const override;

    /** name string */
    std::string name() const override { return "SegmentTag"; }

    /** print content to string */
    std::string toString() const override { return name(); }

  private:
    /** block copy and assignment */
    SegmentTag(const SegmentTag&) = delete;
    SegmentTag& operator=(const SegmentTag&) = delete;

    /** data content */
    std::vector< MuonCombined::MuonSegmentInfo > m_segmentsInfo;
    

  };
  
}


#endif

