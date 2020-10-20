/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRACKSEGMENTMATCHINGTOOL_H
#define MUON_IMUONTRACKSEGMENTMATCHINGTOOL_H
 
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonTrackSegmentMatchingTool("Muon::IMuonTrackSegmentMatchingTool",1,0);

namespace Trk {
  class Track;
}
namespace Muon {
  class MuonSegment;
}

namespace Muon {

  /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */  
  class IMuonTrackSegmentMatchingTool : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    /** @brief clean a track, returns a pointer to a new track if successfull.
	If the input track is does not require cleaning a pointer the the initial track is return in which case the 
	user should not delete the old track! The cleaning will not clean if all the chambers in the exclusions list 
	are marked as to be deleted.
	The caller should ensure the track gets deleted. */
    virtual bool match( const Trk::Track& track, const MuonSegment& segment, bool useTightCuts = false ) const = 0;

    void cleanUp() const {};
  };
  
  inline const InterfaceID& IMuonTrackSegmentMatchingTool::interfaceID()
  {
    return IID_IMuonTrackSegmentMatchingTool;
  }

} // end of name space

#endif 

