#include "TrkKalmanFitter/ForwardKalmanFitter.h"
#include "TrkKalmanFitter/ForwardRefTrackKalmanFitter.h"
#include "TrkKalmanFitter/KalmanFitter.h"
#include "TrkKalmanFitter/KalmanOutlierLogic.h"
#include "TrkKalmanFitter/KalmanOutlierRecovery_InDet.h"
#include "TrkKalmanFitter/KalmanSmoother.h"
#include "TrkKalmanFitter/MeasRecalibSteeringTool.h"
#include "TrkKalmanFitter/KalmanPiecewiseAnnealingFilter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// using namespace Trk;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ForwardKalmanFitter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, ForwardRefTrackKalmanFitter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanFitter )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanOutlierLogic )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanOutlierRecovery_InDet )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanSmoother )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, MeasRecalibSteeringTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, KalmanPiecewiseAnnealingFilter )

DECLARE_FACTORY_ENTRIES(TrkKalmanFitter)
{ // in this package name these methods are registered 
  DECLARE_NAMESPACE_TOOL(Trk, ForwardKalmanFitter)
  DECLARE_NAMESPACE_TOOL(Trk, ForwardRefTrackKalmanFitter)
  DECLARE_NAMESPACE_TOOL(Trk, KalmanFitter)
  DECLARE_NAMESPACE_TOOL(Trk, KalmanOutlierLogic)
  DECLARE_NAMESPACE_TOOL(Trk, KalmanOutlierRecovery_InDet )
  DECLARE_NAMESPACE_TOOL(Trk, KalmanSmoother)
  DECLARE_NAMESPACE_TOOL(Trk, MeasRecalibSteeringTool )
  DECLARE_NAMESPACE_TOOL(Trk, KalmanPiecewiseAnnealingFilter )
}

