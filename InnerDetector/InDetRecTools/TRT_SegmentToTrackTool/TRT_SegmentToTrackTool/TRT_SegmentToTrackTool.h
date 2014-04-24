/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: TRT_SegmentToTrackTool.h
//
// author: Yang Qin 06/03/2014
//         yang.qin@cern.ch
//
//         Thomas Koffas, Markus Elsing
//         Thomas.Koffas@cern.ch
//
// Description: Tool to create SCT track from segments in TRT
//              This tool is based on the TRT_TrigStandaloneTrackFinder algorithm.
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef TRT_SegmentToTrackTool_H
#define TRT_SegmentToTrackTool_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ITRT_SegmentToTrackTool.h"
#include "TrkEventPrimitives/TrackScore.h"

#include <string>

class MsgStream;
class TRT_ID;

namespace Trk {

  class Track;
  class TrackSegment;

  class ITrackFitter;
  class IExtrapolator;
  class IPRD_AssociationTool;

  class ITrackScoringTool;

}

namespace MagField {

  class IMagFieldSvc;

}

namespace InDet {

  //class TrackSegment;

  class TRT_SegmentToTrackTool : 
  virtual public ITRT_SegmentToTrackTool,
    public AthAlgTool
    {

    public:

      //constructor
      TRT_SegmentToTrackTool(const std::string& type, 
				   const std::string& name, 
				   const IInterface* parent );

      virtual ~TRT_SegmentToTrackTool();

      StatusCode initialize();
      StatusCode finalize();

      Trk::Track* segToTrack(const Trk::TrackSegment&);
      /** Check if the TRT segment has already been assigned a Si extension  */
      bool segIsUsed(const Trk::TrackSegment& );
      bool toLower(const Trk::TrackSegment&);
      void resetAll();
      void resetAssoTool();
      /** Add track into the track-score multimap */
      void addNewTrack(Trk::Track*);
      /** Resolve the standalone TRT tracks based on the number of shared TRT hits */
      TrackCollection* resolveTracks();

      int GetnTrkScoreZero() { return m_nTrkScoreZero; }
      int GetnTrkSegUsed()   { return m_nTrkSegUsed; }
      int GetnTRTTrk()       { return m_nTRTTrk; }

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump(std::ostream& out) const;

    private:
      bool                               m_doRefit;//!< Do final careful refit of tracks
      double                             m_fieldUnitConversion;
      double                             m_sharedFrac         ;  //!< Maximum fraction of shared TRT drift circles
      bool                               m_useasso            ;  //!< Use Si cluster association tool
      bool                               m_suppressHoleSearch ;  //!< Suppress hole search during the track summary creation

      ToolHandle<Trk::ITrackFitter>          m_fitterTool    ;  //!< Refitting tool
      ToolHandle<Trk::IExtrapolator>         m_extrapolator  ;  //!< Track extrapolator tool
      ToolHandle<Trk::IPRD_AssociationTool>  m_assotool      ;  //!< Association tool
      ToolHandle<Trk::ITrackScoringTool>     m_scoringTool   ;  //!< Track scoring tool
      ServiceHandle<MagField::IMagFieldSvc>  m_magFieldSvc  ;  //!< Magnetic field service

      /**ID TRT helper*/
      const TRT_ID* m_trtId;

      /** unsorted container of track and track scores.*/
      typedef std::multimap< Trk::TrackScore, Trk::Track* > TrackScoreMap;
      TrackScoreMap m_trackScoreTrackMap;

      /**Tracks that will be passed out of AmbiProcessor.
         Recreated anew each time process() is called*/ 
      TrackCollection* m_finalTracks;

      int m_nTrkScoreZero    ;  //!< Number of tracks rejected by score zero
      int m_nTrkSegUsed      ;  //!< Number of excluded segments by other TRT segments
      int m_nTRTTrk          ;  //!< Number of TRT-only tracks on output

      MsgStream&    dumpevent      (MsgStream&    out) const;
      MsgStream&    dumpconditions (MsgStream&    out) const;

    };

  MsgStream&    operator << (MsgStream&   ,const TRT_SegmentToTrackTool&);
  std::ostream& operator << (std::ostream&,const TRT_SegmentToTrackTool&);

}

#endif //TRT_SegmentToTrackTool_H
