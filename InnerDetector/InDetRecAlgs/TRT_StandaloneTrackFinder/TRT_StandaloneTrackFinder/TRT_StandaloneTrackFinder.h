/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
   Header file for class TRT_StandaloneTrackFinder
  (c) ATLAS Detector software
  Algorithm for Trk::Track production in TRT only
  Version 1.0: 09/28/2007
  Authors    : Thomas Koffas, Markus Elsing
  email      : Thomas.Koffas@cern.ch
**********************************************************************************/

#ifndef TRT_StandaloneTrackFinder_H
#define TRT_StandaloneTrackFinder_H

#include <array>
#include <string>
#include <mutex>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

///Track Collection to store the tracks
#include "TrkTrack/TrackCollection.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "InDetRecToolInterfaces/ITRT_SegmentToTrackTool.h"

///Needed for the TRT track segments
#include "TrkSegment/SegmentCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"


namespace InDet {

  /**
  @class TRT_StandaloneTrackFinder
  
  InDet::TRT_StandaloneTrackFinde is an algorithm which produces tracks
  from stand-alone TRT segments with no Si extension
  @author Thomas.Koffas@cern.ch     
  */

  class ITRT_SegmentToTrackTool;

  class TRT_StandaloneTrackFinder : public AthReentrantAlgorithm
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** Standard Algotithm methods                                   */
      ///////////////////////////////////////////////////////////////////

      TRT_StandaloneTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_StandaloneTrackFinder();
      StatusCode initialize();
      StatusCode execute(const EventContext& ctx) const;
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status                    */
      ///////////////////////////////////////////////////////////////////

      MsgStream& dump(MsgStream& out) const;
      
    private:

      ///////////////////////////////////////////////////////////////////
      /* Private data                                                */
      ///////////////////////////////////////////////////////////////////
     

      int    m_minNumDriftCircles ;  //!< Minimum number of drift circles for TRT segment tracks
      double m_minPt              ;  //!< Minimum pt cut for TRT only (used in preselection * 0.9)
      int    m_matEffects         ;  //!< Particle hypothesis for track fitting
      bool   m_oldLogic           ;  //!< use old transition region hit logic 

      ToolHandle< ITRT_SegmentToTrackTool >      m_segToTrackTool
         {this,"TRT_SegToTrackTool","InDet::TRT_SegmentToTrackTool"}; //!< Segment to track tool

      SG::ReadHandleKey<Trk::SegmentCollection > m_Segments
         {this , "InputSegmentsLocation","TRTSegments"};              //!< TRT segments to use
      SG::ReadHandleKey<Trk::PRDtoTrackMap>      m_prdToTrackMap
         {this,"PRDtoTrackMap",""};                                   //!< map between PRDs and tracks to identify shared hits.

      /**Tracks that will be passed out of AmbiProcessor. 
	 Recreated anew each time process() is called*/
      SG::WriteHandleKey<TrackCollection>        m_finalTracks
         {this,"OutputTracksLocation","TRTStandaloneTracks"};        //!< Output track collection

      /** Global Counters for final algorithm statistics */

      enum ECounter {kNTrtSeg,            //!< Number of input TRT segments to be investigated per event
                     kNUsedSeg,           //!< Number of TRT segments excluded at input (by BackTracking tracks)
                     kNRejectedSeg,       //!< Number of segments rejected in selection at input 
                     kNTrtSegGood,        //!< Number of input TRT segments after cuts per event
                     kNSegFailed,         //!< Number of segments failing to translate to a track (inclusing refit)
                     kNTrkScoreZeroTotal, //!< Number of tracks rejected by score zero
                     kNTrkSegUsedTotal,   //!< Number of excluded segments by other TRT segments
                     kNTRTTrkTotal,       //!< Number of TRT-only tracks on output
                     kNCounter};

      typedef std::array<int,kNCounter> Counter_t;

      mutable std::mutex m_statMutex ATLAS_THREAD_SAFE;
      mutable Counter_t  m_total     ATLAS_THREAD_SAFE {};
      MsgStream& dumpContainerNames(MsgStream& out) const;
      static MsgStream& dumpevent(MsgStream&, const InDet::TRT_StandaloneTrackFinder::Counter_t&);
      friend MsgStream& operator<< (MsgStream&, const InDet::TRT_StandaloneTrackFinder::Counter_t&);      
    };
   
   MsgStream& operator<< (MsgStream&, const InDet::TRT_StandaloneTrackFinder::Counter_t&);
         
}


MsgStream&  operator<< (MsgStream&, const InDet::TRT_StandaloneTrackFinder&);

#endif // TRT_StandaloneTrackFinder_H
