/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"


///Track Collection to store the tracks
#include "TrkTrack/TrackCollection.h"

///Needed for the TRT track segments
#include "TrkSegment/SegmentCollection.h"

///Needed for the track refitter
#include "TrkFitterInterfaces/ITrackFitter.h"

///Needed for the TRT extension tool
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "InDetRecToolInterfaces/ITRT_SeededTrackFinder.h" 
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h"

#include "CxxUtils/checker_macros.h"

#include "IRegionSelector/IRegSelTool.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

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
      /** Standard Algorithm methods                                   */
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

      StatusCode execute_r (const EventContext& ctx) const;
  
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
      ToolHandle<ITRT_TrackExtensionTool>  m_trtExtension
       { this, "TrackExtensionTool", "InDet::TRT_TrackExtensionTool_xk", "TRT track extension tool "};

      SG::ReadHandleKey<Trk::SegmentCollection>  m_SegmentsKey ;  /** TRT segments to use */
      SG::WriteHandleKey<TrackCollection>        m_outTracksKey;

      SG::ReadHandleKey<Trk::PRDtoTrackMap>       m_prdToTrackMap
         {this,"PRDtoTrackMap",""};
      ToolHandle<Trk::IExtendedTrackSummaryTool> m_trackSummaryTool
        {this, "TrackSummaryTool", "InDetTrackSummaryToolNoHoleSearch"};


      ToolHandle<Trk::IExtrapolator>   m_extrapolator; //!< the extrapoator
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      bool   m_SiExtensionCuts; //!< enable cuts after Si segment finding
      double m_minPt;           //!< minimal Pt cut
      double m_maxEta;          //!< maximal Eta cut
      double m_maxRPhiImp;      //!< maximal RPhi impact parameter cut
      double m_maxZImp;         //!< maximal z impact parameter cut

      bool                               m_caloSeededRoI;
      SG::ReadHandleKey<CaloClusterROI_Collection>  m_caloKey
       {this, "InputClusterContainerName", "InDetCaloClusterROIs", "Location of the optional Calo cluster seeds."};

      ToolHandle<IRegSelTool> m_regionSelector{ this, "RegSelTool", "RegSelTool/RegSel_SCT", "Region selector service instance" };
      
      float m_clusterEt;         //!< min Et of CaloCluster for ROISeeding
      float m_deltaEta;          //!< delta Eta used for RoI creation
      float m_deltaPhi;          //!< delta Phi used for RoI creation
      float m_deltaZ;            //!< delta Z used for RoI creation
      /** Global Counters for final algorithm statistics */
      struct Stat_t {
         enum ECounter {
            kNTrtSeg          ,  /** Number of TRT segments to be investigated per event  */
            kNTrtFailSel      ,  /** Number of TRT segments failing input selection */
            kNTrtSegGood      ,  /** Number of TRT segments that will be investigated per event  */
            kNTrtLimit        ,  /** Number of TRT segments lost in busy events */
            kNTrtNoSiExt      ,  /** Number of TRT segments not extended in Si */
            kNExtCut          ,  /** Number of Si extensions failing cuts */
            kNBckTrkTrt       ,  /** Number of back tracks found without a Si extension per event */
            kNTrtExtCalls     ,  /** Number of times the TRT extension is called */
            kNTrtExt          ,  /** Number of good TRT extensions */
            kNTrtExtBad       ,  /** Number of shorter TRT extensions */
            kNTrtExtFail      ,  /** Number of failed TRT extensions */
            kNBckTrkSi        ,  /** Number of back tracks found with Si extension per event */
            kNBckTrk          ,  /** Number of back tracks found with or without Si extension per event */
            kNCounter
         };
         std::array<int,kNCounter> m_counter {};

         Stat_t &operator +=(const Stat_t &a) {
            for (unsigned int i=0; i<a.m_counter.size(); ++i) {
               m_counter[i] += a.m_counter[i];
            }
            return *this;
         }
      };

      mutable std::mutex m_statMutex ATLAS_THREAD_SAFE;
      mutable Stat_t m_totalStat ATLAS_THREAD_SAFE;

      ///////////////////////////////////////////////////////////////////
      /** Protected methods                                            */
      ///////////////////////////////////////////////////////////////////

      /** Merge a TRT track segment and a Si track component into one global ID track */
      Trk::Track*                   mergeSegments(const Trk::Track&,const Trk::TrackSegment&) const;

      /** Merge a TRT track extension and a Si track component into one global ID track */
      Trk::Track*                   mergeExtension(const Trk::Track&,std::vector<const Trk::MeasurementBase*>&) const;

      /** Transform a TRT track segment into a track  */
        Trk::Track*                 segToTrack(const EventContext&, const Trk::TrackSegment&) const;

      /** Do some statistics analysis at the end of each event */
      void                 Analyze(TrackCollection*) const;

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out,const InDet::TRT_SeededTrackFinder::Stat_t &stat) const;

    };
}
#endif // TRT_SeededTrackFinder_H
