/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
   Header file for class TRT_SeededTrackFinder
  (c) ATLAS Detector software
  Algorithm for Trk::Track production in SCT and Pixels
  Version 1.0: 04/12/2006
  Authors    : Thomas Koffas, Markus Elsing
  email      : Thomas.Koffas@cern.ch
**********************************************************************************/

#ifndef TRT_SeededTrackFinder_H
#define TRT_SeededTrackFinder_H

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "InDetRecToolInterfaces/ITRT_SeededTrackFinder.h" 

///Track Collection to store the tracks
#include "TrkTrack/TrackCollection.h"

///Needed for the TRT track segments
#include "TrkSegment/SegmentCollection.h"

///Needed for the track refitter
#include "TrkFitterInterfaces/ITrackFitter.h"

///Needed for the TRT extension tool
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

class MsgStream;

namespace InDet {

  /**
  @class TRT_SeededTrackFinder
  
  InDet::TRT_SeededTrackFinde is an algorithm which produces tracks
  moving outside-in in the Inner Detector.
  */

  class TRT_SeededTrackFinder : public AthAlgorithm 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      /** Standard Algotithm methods                                   */
      ///////////////////////////////////////////////////////////////////

      TRT_SeededTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_SeededTrackFinder() {}
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();

      ///////////////////////////////////////////////////////////////////
      /** Print internal tool parameters and status                    */
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      /* Protected data                                                */
      ///////////////////////////////////////////////////////////////////
     
      int                                m_nprint           ;
      int                                m_ntracks          ;  /** Number of tracks found */
      bool                               m_doRefit          ;  /** Do final careful refit of tracks */
      bool                               m_doExtension      ;  /** Find the TRT extension of the Si track segment */
      bool                               m_rejectShortExten ;  /** use extension only if better than original track */
      bool                               m_doStat           ;  /** Statistics of final tracks */
      bool                               m_saveTRT          ;  /** Output stand-alone TRT segments */
      int                                m_MaxSegNum        ;  /** Maximum number of segments to be handled */
      unsigned int                       m_minTRTonSegment  ;  /** Minimum number of TRT hits on segment */
      unsigned int                       m_minTRTonly       ;  /** Minimum number of TRT hits on TRT only */

      ToolHandle<ITRT_SeededTrackFinder>   m_trackmaker     ;  /** Track maker tool */
      ToolHandle<Trk::ITrackFitter>        m_fitterTool     ;  /** Refitting tool */
      ToolHandle<ITRT_TrackExtensionTool>  m_trtExtension   ;  /** TRT track extension tool */

      SG::ReadHandle<Trk::SegmentCollection> m_Segments     ;  /** TRT segments to use */
      SG::WriteHandle<TrackCollection>       m_outTracks   ;


      ToolHandle<Trk::IExtrapolator>   m_extrapolator; //!< the extrapoator
      ServiceHandle<IBeamCondSvc>      m_iBeamCondSvc; //!< pointer to the beam condition service
      bool   m_SiExtensionCuts; //!< enable cuts after Si segment finding
      double m_minPt;           //!< minimal Pt cut
      double m_maxEta;          //!< maximal Eta cut
      double m_maxRPhiImp;      //!< maximal RPhi impact parameter cut
      double m_maxZImp;         //!< maximal z impact parameter cut

      /** Global Counters for final algorithm statistics */
      int m_nTrtSeg          ;  /** Number of TRT segments to be investigated per event  */
      int m_nTrtFailSel      ;  /** Number of TRT segments failing input selection */
      int m_nTrtSegGood      ;  /** Number of TRT segments that will be investigated per event  */
      int m_nTrtLimit        ;  /** Number of TRT segments lost in busy events */
      int m_nTrtNoSiExt      ;  /** Number of TRT segments not extended in Si */
      int m_nExtCut          ;  /** Number of Si extensions failing cuts */
      int m_nBckTrkTrt       ;  /** Number of back tracks found without a Si extension per event */
      int m_nTrtExtCalls     ;  /** Number of times the TRT extension is called */
      int m_nTrtExt          ;  /** Number of good TRT extensions */
      int m_nTrtExtBad       ;  /** Number of shorter TRT extensions */
      int m_nTrtExtFail      ;  /** Number of failed TRT extensions */
      int m_nBckTrkSi        ;  /** Number of back tracks found with Si extension per event */
      int m_nBckTrk          ;  /** Number of back tracks found with or without Si extension per event */

      int m_nTrtSegTotal     ;  /** Total number of TRT segments to be investigated  */
      int m_nTrtFailSelTotal ;  /** Total number of TRT segments failing input selection */
      int m_nTrtSegGoodTotal ;  /** Total number of TRT segments that will be investigated  */
      int m_nTrtLimitTotal   ;  /** Total number of TRT segments lost in busy events */
      int m_nTrtNoSiExtTotal ;  /** Total number of TRT segments not extended in Si */
      int m_nExtCutTotal     ;  /** Total number of Si extensions failing cuts */
      int m_nBckTrkTrtTotal  ;  /** Total number of back tracks found without a Si extension */
      int m_nTrtExtCallsTotal;  /** Total number of times the TRT extension is called */
      int m_nTrtExtTotal     ;  /** Total number of good TRT extensions */
      int m_nTrtExtBadTotal  ;  /** Total number of shorter TRT extensions */
      int m_nTrtExtFailTotal ;  /** Total number of failed TRT extensions */
      int m_nBckTrkSiTotal   ;  /** Total number of back tracks found with Si extension */
      int m_nBckTrkTotal     ;  /** Total number of back tracks found with or without Si extension  */

      ///////////////////////////////////////////////////////////////////
      /** Protected methods                                            */
      ///////////////////////////////////////////////////////////////////

      /** Merge a TRT track segment and a Si track component into one global ID track */
      Trk::Track*                   mergeSegments(const Trk::Track&,const Trk::TrackSegment&);

      /** Merge a TRT track extension and a Si track component into one global ID track */
      Trk::Track*                   mergeExtension(const Trk::Track&,std::vector<const Trk::MeasurementBase*>&);

      /** Transform a TRT track segment into a track  */
      Trk::Track*                   segToTrack(const Trk::TrackSegment&);

      /** Do some statistics analysis at the end of each event */
      void                 Analyze(TrackCollection*);

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

    };
  MsgStream&    operator << (MsgStream&   ,const TRT_SeededTrackFinder&);
  std::ostream& operator << (std::ostream&,const TRT_SeededTrackFinder&); 
}
#endif // TRT_SeededTrackFinder_H
