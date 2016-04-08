/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
   Header file for class TRT_TrigStandaloneTrackFinder
  (c) ATLAS Detector software
  Algorithm for Trk::Track production in TRT only - trigger version of TRT_StandaloneTrackFinder by Thomas Koffas
  Version 1.0: 16/05/2008
  Authors    : Iwona Grabowska-Bold
  email      : Iwona.Grabowska-Bold@cern.ch
**********************************************************************************/

#ifndef TRT_TrigStandaloneTrackFinder_H
#define TRT_TrigStandaloneTrackFinder_H

#include "GaudiKernel/ToolHandle.h"

///Track events
#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"

///Track Collection to store the tracks
#include "TrkTrack/TrackCollection.h"

#include "TrkSegment/SegmentCollection.h"

//Trigger
#include "TrigInterfaces/FexAlgo.h"

namespace InDet {

  /**
  @class TRT_TrigStandaloneTrackFinder
  
  InDet::TRT_TrigStandaloneTrackFinde is an algorithm which produces tracks
  from stand-alone TRT segments with no Si extension
  @author Thomas.Koffas@cern.ch     
  */

  class ITRT_SegmentToTrackTool;

  class TRT_TrigStandaloneTrackFinder : public HLT::FexAlgo 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** Standard Algotithm methods                                   */
      ///////////////////////////////////////////////////////////////////

      TRT_TrigStandaloneTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_TrigStandaloneTrackFinder();
      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
      HLT::ErrorCode hltFinalize();

      //parameters for monitoring
      int m_nTRTTracks         ; //!< Number of TRT standalone tracks
      //int m_nTrtSeg          ;  //!< Number of TRT segments to be investigated per event

      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status                    */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    private:

      ///////////////////////////////////////////////////////////////////
      /* Private data                                                */
      ///////////////////////////////////////////////////////////////////
     
      int                                m_outputlevel        ;  //!< Print level for debugging
      int                                m_nprint             ;
      int                                m_ntracks            ;  //!< Number of tracks found

      int                                m_minNumDriftCircles ;  //!< Minimum number of drift circles for TRT segment tracks

      bool                               m_resetPRD           ;  /** Reset PRD association tool during the sub-detector pattern */

      int                                m_matEffects         ;  //!< Particle hypothesis for track fitting

      //StoreGateSvc*                         m_store_gate    ;  //!< Pointer to store gate 

      ToolHandle< ITRT_SegmentToTrackTool > m_segToTrackTool; //!< Segment to track tool
      
      const Trk::SegmentCollection      *m_Segments         ;  //!< TRT segments to use

      /**Tracks that will be passed out of AmbiProcessor. 
	 Recreated anew each time process() is called*/ 
      TrackCollection* m_finalTracks;

      /** Global Counters for final algorithm statistics */
      int m_nTrtSeg          ;  //!< Number of TRT segments to be investigated per event
      int m_nTrtSegGood      ;  //!< Number of TRT segments that will be investigated per event
      int m_nBckTrk          ;  //!< Total number of TRT segment back tracks found per event
      int m_nUsedSeg         ;  //!< Total number of TRT segments assigned Si extensions per event

      int m_nTrtSegTotal     ;  //!< Total number of TRT segments to be investigated
      int m_nTrtSegGoodTotal ;  //!< Total number of TRT segments that will be investigated
      int m_nBckTrkTotal     ;  //!< Total number of TRT segment back tracks found
      int m_nUsedSegTotal    ;  //!< Total number of TRT segments assigned Si extensions
      int m_ntimesInvoked    ;  //!< Keep track how many times the algo was invoked

      ///////////////////////////////////////////////////////////////////
      /** Protected methods                                            */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const TRT_TrigStandaloneTrackFinder&);
  std::ostream& operator << (std::ostream&,const TRT_TrigStandaloneTrackFinder&); 
}
#endif // TRT_TrigStandaloneTrackFinder_H
