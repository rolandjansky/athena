/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventPrimitives/TrackScore.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

class MsgStream;
class TRT_ID;

namespace Trk {

  class Track;
  class TrackSegment;

  class ITrackFitter;
  class IExtrapolator;

  class ITrackScoringTool;

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

      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;

      virtual Trk::Track* segToTrack(const EventContext& ctx, const Trk::TrackSegment&) const override;
      /** Check if the TRT segment has already been assigned a Si extension  */
      virtual bool segIsUsed(const Trk::TrackSegment&,
                             const Trk::PRDtoTrackMap *prd_to_track_map) const override;
      virtual bool toLower(const Trk::TrackSegment&) const override;

      /** Add track into the track-score multimap */
      virtual void addNewTrack(Trk::Track*,
                               ITRT_SegmentToTrackTool::EventData &event_data) const override;
      /** Resolve the standalone TRT tracks based on the number of shared TRT hits */
      virtual TrackCollection* resolveTracks(const Trk::PRDtoTrackMap *prd_to_track_map,
                                             ITRT_SegmentToTrackTool::EventData &event_data) const override;

      virtual MsgStream&    dump     (MsgStream&    out) const override;
      virtual std::ostream& dump(std::ostream& out) const override;

    private:
      bool                               m_doRefit;//!< Do final careful refit of tracks
      double                             m_fieldUnitConversion;
      double                             m_sharedFrac         ;  //!< Maximum fraction of shared TRT drift circles
      bool                               m_suppressHoleSearch ;  //!< Suppress hole search during the track summary creation

      ToolHandle<Trk::ITrackFitter>          m_fitterTool    ;  //!< Refitting tool
      ToolHandle<Trk::IExtrapolator>         m_extrapolator  ;  //!< Track extrapolator tool

      ToolHandle<Trk::IPRDtoTrackMapTool>  m_assoTool
         {this, "AssociationTool", "InDet::InDetPRDtoTrackMapToolGangedPixels" };
      ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool
        {this, "TrackSummaryTool", "InDetTrackSummaryToolNoHoleSearch"};

      ToolHandle<Trk::ITrackScoringTool>     m_scoringTool   ;  //!< Track scoring tool

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      // Read handle for conditions object to get the field cache
      SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                                 "Name of the Magnetic Field conditions object key"};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      /**ID TRT helper*/
      const TRT_ID* m_trtId;

      /**Tracks that will be passed out of AmbiProcessor.
         Recreated anew each time process() is called*/ 
      MsgStream&    dumpevent      (MsgStream&    out) const;
      MsgStream&    dumpconditions (MsgStream&    out) const;

    };

   ///////////////////////////////////////////////////////////////////
   // Overload of << operator for MsgStream and  std::ostream
   ///////////////////////////////////////////////////////////////////

   MsgStream&    operator << (MsgStream&   ,const ITRT_SegmentToTrackTool&);
   std::ostream& operator << (std::ostream&,const ITRT_SegmentToTrackTool&);

   ///////////////////////////////////////////////////////////////////
   // Inline methods
   ///////////////////////////////////////////////////////////////////

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

#endif //TRT_SegmentToTrackTool_H
