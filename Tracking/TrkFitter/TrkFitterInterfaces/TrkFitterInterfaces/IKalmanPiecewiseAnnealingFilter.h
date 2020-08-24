/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IKalmanPiecewiseAnnealingFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IKALMANPIECEWISEANNEALINGFILTER_H
#define IKALMANPIECEWISEANNEALINGFILTER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" // can not fwd-declare
#include "TrkFitterUtils/ProtoTrackStateOnSurface.h"
#include "TrkFitterUtils/FitterStatusCode.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace Trk 
{
  typedef std::vector<Trk::ProtoTrackStateOnSurface> Trajectory;

  class ProtoTrajectoryUtility;
  class IExtrapolator;
  class IUpdator;
  class IMeasurementRecalibrator;
  class IDynamicNoiseAdjustor;

  static const InterfaceID IID_IKalmanPiecewiseAnnealingFilter("Trk::IKalmanPiecewiseAnnealingFilter", 1, 0);

  class IKalmanPiecewiseAnnealingFilter : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( );

    virtual StatusCode  configureWithTools(const IExtrapolator*,
                                           const IUpdator*,
                                           const IMeasurementRecalibrator*,
                                           const ProtoTrajectoryUtility*,
                                           const IDynamicNoiseAdjustor* = nullptr ) = 0;

    /** @brief run DAF on the full trajectory, starting from first fittable state and
               parameters therein, do not do last smoother */
    virtual FitterStatusCode
      filterTrajectory (Trajectory& trajectory,
                        const ParticleHypothesis& ) const =0;

    /** @brief run DAT on a piece of the current trajectory, starting from start and
               until size is reached, modifying the pred/upd parameters, do not do last smoother  */
    virtual FitterStatusCode
      filterTrajectoryPiece (Trajectory& trajectory,
                             Trajectory::iterator&,
                             const TrackParameters*&,
                             const TrackParameters*&,
                             int,
                             const ParticleHypothesis& ) const =0;

  };

  inline const InterfaceID& Trk::IKalmanPiecewiseAnnealingFilter::interfaceID()
    { 
      return IID_IKalmanPiecewiseAnnealingFilter; 
    }

} // end of namespace

#endif 
