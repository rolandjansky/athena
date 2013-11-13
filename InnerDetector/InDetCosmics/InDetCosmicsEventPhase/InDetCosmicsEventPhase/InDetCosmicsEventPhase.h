/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkTrack/Track.h"
#include "TrkSegment/TrackSegment.h"
#include "InDetCosmicsEventPhase/IInDetCosmicsEventPhaseTool.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
//class TrackCollection;
class ITRT_CalDbSvc ;

namespace Trk
{
  class ITrackSummaryTool;
}
namespace InDet 
{
  class IInDetCosmicsEventPhaseTool;
}

namespace InDet
{
  class InDetCosmicsEventPhase:public AthAlgorithm 
    {
    public:
      InDetCosmicsEventPhase (const std::string& name, ISvcLocator* pSvcLocator);
      StatusCode initialize();
      StatusCode beginRun();
      StatusCode execute();
      StatusCode finalize();
      void storePhase();
      
    private:
      std::vector<std::string> m_inputtracksnames;
      std::string m_inputsegmentsname;
      
      int m_event;
      double m_phase;
      
      ServiceHandle<ITRT_CalDbSvc>       m_trtconddbsvc ;//!< TRT Calibration DB tool
      ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool; //<! Track summary tool
      ToolHandle<InDet::IInDetCosmicsEventPhaseTool> m_eventPhaseTool; //<! Cosmics Event Phase tool
    };
}
