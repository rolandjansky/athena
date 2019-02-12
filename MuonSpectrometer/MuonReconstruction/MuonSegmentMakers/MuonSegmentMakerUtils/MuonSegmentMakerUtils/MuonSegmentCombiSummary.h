/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTCOMBISUMMARY_H 
#define MUON_MUONSEGMENTCOMBISUMMARY_H

#include "MuonIdHelpers/MuonStationIndex.h"

#include <vector>
#include <set>

namespace Muon{

  class MuonSegment;
  class MuonSegmentCombination;

  class MuonSegmentCombiSummary {
  public:
    // vector of segments
    typedef std::vector<MuonSegment*>       SegVec;
    typedef SegVec::iterator                SegIt;
    typedef SegVec::const_iterator          SegCit;

    // vector of SegVec per chamber, indexed using MuonStationIndex::ChIndex
    typedef std::vector< SegVec >           ChSegVec;
    typedef ChSegVec::iterator              ChSegIt;
    typedef ChSegVec::const_iterator        ChSegCit;
    
  public:
    MuonSegmentCombiSummary( const MuonSegmentCombination& segment );
    virtual ~MuonSegmentCombiSummary();

    /** access to combi */
    const MuonSegmentCombination& segmentCombination() { return *m_combi; }
    

    // access to all segments
    ChSegVec& segmentsPerLayer() { return m_segmentsPerChamberLayer; }

    // access to segments in a given chamber layer
    SegVec& chamberSegments( unsigned int chIndex ) { return m_segmentsPerChamberLayer[chIndex]; }

    // counter for station layers
    std::set<MuonStationIndex::StIndex> stations;        //!< number of station layers
    std::set<MuonStationIndex::StIndex> stationsGood;    //!< number of station layers with good segment
    std::set<MuonStationIndex::StIndex> stationsTrigger; //!< number of station layers with trigger hit
    
    ChSegVec                      m_segmentsPerChamberLayer; 
    const MuonSegmentCombination* m_combi;
    const MuonSegmentCombination* combiOriginal;
    
    unsigned int nsegments;
  };

}


#endif
