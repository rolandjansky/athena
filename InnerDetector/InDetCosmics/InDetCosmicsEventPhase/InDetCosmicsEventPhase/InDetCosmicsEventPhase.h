/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetCosmicsEventPhase/IInDetCosmicsEventPhaseTool.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/WriteHandleKey.h"
#include "CommissionEvent/ComTime.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
//class TrackCollection;
class ITRT_CalDbTool ;

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
      StatusCode execute();
      StatusCode finalize();
      StatusCode storePhase();
      
    private:
      double	m_phase = 0.0;

      SG::ReadHandleKeyArray<TrackCollection>	m_readKey_tracks 	{this, "InputTracksNames"	,{"Tracks"}, "Tracks to extract event phase" };
      SG::WriteHandleKey<ComTime> 		m_writeKey_TRTPhase	{this, "EventPhaseName"		,"TRT_Phase", "TRT Event Phase name to store" };

      ToolHandle<ITRT_CalDbTool>       m_caldbtool ;//!< TRT Calibration DB tool
      ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool; //<! Track summary tool
      ToolHandle<InDet::IInDetCosmicsEventPhaseTool> m_eventPhaseTool; //<! Cosmics Event Phase tool
    };
}
