/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class MsgStream;

namespace InDet {

  static const InterfaceID IID_ITRT_SegmentToTrackTool("InDet::ITRT_SegmentToTrackTool", 1, 0);

  class ITRT_SegmentToTrackTool : virtual public IAlgTool {

  public:

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize () = 0;
    virtual StatusCode finalize   () = 0;

    virtual Trk::Track* segToTrack(const Trk::TrackSegment&) = 0;
    /** Check if the TRT segment has already been assigned a Si extension  */
    virtual bool segIsUsed(const Trk::TrackSegment&) = 0;
    virtual bool toLower(const Trk::TrackSegment&) = 0;
    virtual void resetAll() = 0;
    virtual void resetAssoTool() = 0;
    /** Add track into the track-score multimap */
    virtual void addNewTrack(Trk::Track*) = 0;
    /** Resolve the standalone TRT tracks based on the number of shared TRT hits */
    virtual TrackCollection* resolveTracks() = 0;

    virtual int GetnTrkScoreZero() = 0;
    virtual int GetnTrkSegUsed() = 0;
    virtual int GetnTRTTrk() = 0;

    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
   
    virtual MsgStream&    dump(MsgStream&    out) const = 0;
    virtual std::ostream& dump(std::ostream& out) const = 0;

  };

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator for MsgStream and  std::ostream
  ///////////////////////////////////////////////////////////////////
 
  MsgStream&    operator << (MsgStream&   ,const ITRT_SegmentToTrackTool&);
  std::ostream& operator << (std::ostream&,const ITRT_SegmentToTrackTool&);

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const InterfaceID& InDet::ITRT_SegmentToTrackTool::interfaceID(){
    return IID_ITRT_SegmentToTrackTool;
  }

  ///////////////////////////////////////////////////////////////////
  // Overload of << operator MsgStream
  ///////////////////////////////////////////////////////////////////
   
  inline MsgStream& operator    <<     (MsgStream& sl,const ITRT_SegmentToTrackTool& se)
  { 
    return se.dump(sl); 
  }
  ///////////////////////////////////////////////////////////////////
  // Overload of << operator std::ostream
  ///////////////////////////////////////////////////////////////////
 
  inline std::ostream& operator <<     (std::ostream& sl,const ITRT_SegmentToTrackTool& se)
  {
    return se.dump(sl); 
  }

}

#endif //ITRT_SegmentToTrackTool_H
