/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_TrigTrackSegmentsFinder_H
#define TRT_TrigTrackSegmentsFinder_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetRecToolInterfaces/ITRT_TrackSegmentsMaker.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

#include "IRegionSelector/IRegSelTool.h"

#include <iosfwd>


class TrigTimer;

namespace InDet {

  // Class-algorithm for track finding in TRT
  // 
  class TRT_TrigTrackSegmentsFinder : public HLT::FexAlgo 
    {
    
      ///////////////////////////////////////////////////////////////////
      // Public methods:
      ///////////////////////////////////////////////////////////////////
      
    public:
      
      ///////////////////////////////////////////////////////////////////
      // Standard Algorithm methods
      ///////////////////////////////////////////////////////////////////

      TRT_TrigTrackSegmentsFinder(const std::string &name, ISvcLocator *pSvcLocator);
      virtual ~TRT_TrigTrackSegmentsFinder() {};
      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
      HLT::ErrorCode hltFinalize();

      //parameters for monitoring
      double m_RoIPhi;
      double m_RoIEta;
      int m_nTRTSegs;

      ///////////////////////////////////////////////////////////////////
      // Print internal tool parameters and status
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dump     (MsgStream&    out) const;
      std::ostream& dump     (std::ostream& out) const;

    protected:

      ///////////////////////////////////////////////////////////////////
      // Protected data 
      ///////////////////////////////////////////////////////////////////
     
      int                         m_outputlevel{}     ; // Print level for debug
      int                         m_nprint {}         ; // Kind of  print
      int                         m_nsegments{}       ; // Number segments
      int                         m_nsegmentsTotal{}   ; // Number segments
      ToolHandle<ITRT_TrackSegmentsMaker> m_segmentsMakerTool; // TRT segments maker


      ///////////////////////////////////////////////////////////////////
      // Protected methods
      ///////////////////////////////////////////////////////////////////

      MsgStream&    dumptools(MsgStream&    out) const;
      MsgStream&    dumpevent(MsgStream&    out) const;

      //!< Trigger part
      ToolHandle<IRegSelTool>     m_regionSelector{ this, "RegSelTool", "RegSelTool/RegSelTool_TRT" };     //!< region selector service
      double                  m_etaHalfWidth{};   //!< ROI half-width in eta.
      double                  m_phiHalfWidth{};   //!< ROI half-width in phi.
      bool m_doFullScan{}; 			//!< supports FullScan mode

      //!< Timing
      TrigTimer     *m_timerRegSel{};
      TrigTimer     *m_timerFind{};
      TrigTimer     *m_timerSegMaker{};
      TrigTimer     *m_timerMainLoop{};
    };
  MsgStream&    operator << (MsgStream&   ,const TRT_TrigTrackSegmentsFinder&);
  std::ostream& operator << (std::ostream&,const TRT_TrigTrackSegmentsFinder&); 
}
#endif // TRT_TrigTrackSegmentsFinder_H
