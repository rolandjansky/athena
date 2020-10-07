/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
/**
// filename: TRT_TrigSeededTrackFinder.h
//
// author: Adapted for the Event Filter: Iwona Grabowska-Bold, March 2007
//         Iwona.Grabowska@cern.ch
//
// Description:  Trigger version of the TRT_SeededTrackFinder
//               (see original package documentation).
//
// -------------------------------
// ATLAS Collaboration
*/
//***************************************************************************

#ifndef TRT_TrigSeededTrackFinder_H
#define TRT_TrigSeededTrackFinder_H

#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ITRT_SeededTrackFinder.h" 

///Needed for the track refitter
#include "TrkFitterInterfaces/ITrackFitter.h"

///Needed for the TRT extension tool
#include "InDetRecToolInterfaces/ITRT_TrackExtensionTool.h"

///Needed for the TRT track segments
#include "TrkSegment/SegmentCollection.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

//!< Trigger specific stuff
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigInterfaces/FexAlgo.h"
#include "IRegionSelector/IRegSelTool.h"


namespace InDet {


  // Class-algorithm for track finding in Pixels and SCT
  // initiated by TRT track seeds
  // 
  class TRT_TrigSeededTrackFinder : public HLT::FexAlgo {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algotithm methods
      ///////////////////////////////////////////////////////////////////

      TRT_TrigSeededTrackFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_TrigSeededTrackFinder() {}
      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
      HLT::ErrorCode hltFinalize();

      ///////////////////////////////////////////////////////////////////
      // Generalpublic methods
      ///////////////////////////////////////////////////////////////////
     
      //typedef std::multimap<double, Trk::Track*> TrackMap   ;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

  protected:
      ToolHandle<IRegSelTool>     m_regionSelector{ this, "RegSelTool", "RegSelTool/RegSelTool_TRT" };      //!< region selector service
      
      double       m_etaHalfWidth{};   //!< ROI half-width in eta.
      double       m_phiHalfWidth{};   //!< ROI half-width in phi.
      bool         m_doFullScan{};    //!< FullScan mode
      
      int                     m_nDetElPixel{};  //Number of pixel det elements
      int                     m_nDetElSCT{};    //Number of SCT det elements
      double                  m_RoIEta{};      
      double                  m_RoIPhi{};
      
      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     
      int                                m_outputlevel{}      ;  // Print level for debug
      int                                m_nprint{}           ;  // Kind of  print    
      int                                m_ntracks{}          ;  // Number of tracks found

      bool                               m_doRefit{}          ;  // Do final careful refit of tracks
      bool                               m_doExtension{}      ; // Find the TRT extension of the Si track segment 
      bool                               m_saveTRT{}          ;  // Output stand-alone TRT segments


      ToolHandle<ITRT_SeededTrackFinder>   m_trackmaker     ;  //Track maker tool
      ToolHandle<Trk::ITrackFitter>        m_fitterTool     ;  //Refitting tool
      ToolHandle<ITRT_TrackExtensionTool>  m_trtExtension   ; // TRT track extension tool

      StringProperty m_prdToTrackMap
         {this,"PRDtoTrackMap",""};                           //!< map between PRDs and tracks to identify shared hits.

      const Trk::SegmentCollection      *m_Segments{}         ;  // Segments to use

      ///Sets and maps to use for dropping dublicates
      //typedef std::set<const Trk::PrepRawData*> PrdSignature;
      //typedef std::set<PrdSignature>            PrdSignatureSet;
      //PrdSignatureSet                           m_prdSigSet;

      ///Track quality cuts
      int                                     m_minNDF{}       ;  //Minimum number of degrees of freedom

      ///Counters
      int m_nTrtSeg{}          ;  //Number of TRT segments to be investigated per event
      int m_nTrtSegGood{}      ;  //Number of TRT segments that will be investigated per event
      int m_nBckTrk{}          ;  //Total number of back tracks found with or without Si extension per event
      int m_nBckTrkTrt{}       ;  //Number of back tracks found without a Si extension per event
      int m_nBckTrkSi{}        ;  //Number of back tracks found with Si extension per event

      int m_nTrtSegTotal{}     ;  // Total number of TRT segments to be investigated
      int m_nTrtSegGoodTotal{} ;  // Total number of TRT segments that will be investigated
      int m_nBckTrkTotal{}     ;  // Total number of back tracks found with or without Si extension
      int m_nBckTrkTrtTotal{}  ;  // Total number of back tracks found without a Si extension
      int m_nBckTrkSiTotal{}   ;  // Total number of back tracks found with Si extension

      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////
      /** Merge a TRT track segment and a Si track component into one global ID track */
      Trk::Track*                   mergeSegments(const Trk::Track&,const Trk::TrackSegment&);
      /** Merge a TRT track extension and a Si track component into one global ID track */
      Trk::Track* mergeExtension(const Trk::Track&,std::vector<const Trk::MeasurementBase*>&);
      /** Transform a TRT track segment into a track */
      Trk::Track*                   segToTrack(const Trk::TrackSegment&);

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;
      
      //Timing
      TrigTimer      *m_timerRegSel{};

    };
  MsgStream&    operator << (MsgStream&   ,const TRT_TrigSeededTrackFinder&);
  std::ostream& operator << (std::ostream&,const TRT_TrigSeededTrackFinder&); 
}
#endif // TRT_TrigSeededTrackFinder_H
