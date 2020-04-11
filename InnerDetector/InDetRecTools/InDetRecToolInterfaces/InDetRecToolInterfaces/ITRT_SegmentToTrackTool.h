/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: ITRT_SegmentToTrackTool.h
//
// author: Yang Qin 06/03/2014
//         yang.qin@cern.ch
//
// Description: The interface for TRT_SegmentToTrackTool
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef ITRT_SegmentToTrackTool_H
#define ITRT_SegmentToTrackTool_H

#include "GaudiKernel/IAlgTool.h"

#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "GaudiKernel/EventContext.h"
#include <utility>

class MsgStream;

namespace Trk {
  class PRDtoTrackMap;
}

namespace InDet {

  static const InterfaceID IID_ITRT_SegmentToTrackTool("InDet::ITRT_SegmentToTrackTool", 1, 0);

  class ITRT_SegmentToTrackTool : virtual public IAlgTool {

  public:

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize () = 0;
    virtual StatusCode finalize   () = 0;

    struct EventData {
       std::vector< std::pair<Trk::TrackScore, Trk::Track*> > m_trackScores;
       enum ECounter {knTrkScoreZero, ///< Number of tracks rejected by score zero
                      knTrkSegUsed,   ///< Number of excluded segments by other TRT segments
                      knTRTTrk,       ///< Number of TRT-only tracks on output
                      kNCounter};
       std::array<int,kNCounter>  m_counter {};
    };

    virtual Trk::Track* segToTrack(const EventContext&, const Trk::TrackSegment&) const = 0;

    /** Check if the TRT segment has already been assigned a Si extension  */
    virtual bool segIsUsed(const Trk::TrackSegment&,
                           const Trk::PRDtoTrackMap *) const = 0;

    virtual bool toLower(const Trk::TrackSegment&) const = 0;

    /** Add track into the track-score multimap */
    virtual void addNewTrack(Trk::Track*,
                             ITRT_SegmentToTrackTool::EventData &event_data) const = 0;

    /** Resolve the standalone TRT tracks based on the number of shared TRT hits */
    virtual TrackCollection* resolveTracks(const Trk::PRDtoTrackMap *,
                                           ITRT_SegmentToTrackTool::EventData &event_data) const = 0;

    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////

    virtual MsgStream&    dump(MsgStream&    out) const = 0;
    virtual std::ostream& dump(std::ostream& out) const = 0;

  };

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& InDet::ITRT_SegmentToTrackTool::interfaceID(){
    return IID_ITRT_SegmentToTrackTool;
  }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////

}

#endif //ITRT_SegmentToTrackTool_H
