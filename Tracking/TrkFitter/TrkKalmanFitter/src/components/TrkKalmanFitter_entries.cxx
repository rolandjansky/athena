#include "TrkKalmanFitter/ForwardKalmanFitter.h"
#include "TrkKalmanFitter/ForwardRefTrackKalmanFitter.h"
#include "TrkKalmanFitter/KalmanFitter.h"
#include "TrkKalmanFitter/KalmanOutlierLogic.h"
#include "TrkKalmanFitter/KalmanOutlierRecovery_InDet.h"
#include "TrkKalmanFitter/KalmanSmoother.h"
#include "TrkKalmanFitter/MeasRecalibSteeringTool.h"
#include "TrkKalmanFitter/KalmanPiecewiseAnnealingFilter.h"

DECLARE_COMPONENT( Trk::ForwardKalmanFitter )
DECLARE_COMPONENT( Trk::ForwardRefTrackKalmanFitter )
DECLARE_COMPONENT( Trk::KalmanFitter )
DECLARE_COMPONENT( Trk::KalmanOutlierLogic )
DECLARE_COMPONENT( Trk::KalmanOutlierRecovery_InDet )
DECLARE_COMPONENT( Trk::KalmanSmoother )
DECLARE_COMPONENT( Trk::MeasRecalibSteeringTool )
DECLARE_COMPONENT( Trk::KalmanPiecewiseAnnealingFilter )

