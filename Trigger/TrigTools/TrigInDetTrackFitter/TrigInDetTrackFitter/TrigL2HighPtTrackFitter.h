/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_HIGH_PT_TRACK_FITTER_H__
#define __TRIG_L2_HIGH_PT_TRACK_FITTER_H__ 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkDistributedKalmanFilter/TrkBaseNode.h"
#include "TrkDistributedKalmanFilter/TrkTrackState.h"
#include "TrigInDetTrackFitter/ITrigL2FastExtrapolationTool.h"
#include "TrigInDetTrackFitter/ITrigL2TrackFittingTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include <vector>

namespace Trk {	
  class IRIO_OnTrackCreator;
}

class TrigL2HighPtTrackFitter : virtual public ITrigL2TrackFittingTool, public AthAlgTool {
 public:
      
  // standard AlgTool methods
  TrigL2HighPtTrackFitter(const std::string&,const std::string&,const IInterface*);
  virtual ~TrigL2HighPtTrackFitter(){};
		
  // standard Athena methods
  StatusCode initialize();
  StatusCode finalize();
  Trk::TrkTrackState* fit(Trk::TrkTrackState*, std::vector<Trk::TrkBaseNode*>&, bool runSmoother=true);

 private:

  void m_recalibrateFilteringNode(Trk::TrkBaseNode*, Trk::TrkTrackState*);

  bool m_recalibrate;

  ToolHandle<ITrigL2FastExtrapolationTool> m_fastExtrapolator;
  ToolHandle<Trk::IRIO_OnTrackCreator>       m_ROTcreator;
};

#endif 
