/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IGsfSmoother.h
 * @date   Thursday 8th January 2009
 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Abstract interface for the GSF smoother
 */

#ifndef TrkIGsfSmoother_H
#define TrkIGsfSmoother_H

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class IMultiStateMeasurementUpdator;
class CaloCluster_OnTrack;

static const InterfaceID InterfaceID_GsfSmoother("GsfSmoother", 1, 0);

class IGsfSmoother : virtual public IAlgTool
{

public:
  /** AlgTool interface method */
  static const InterfaceID& interfaceID() { return InterfaceID_GsfSmoother; };

  /** Virtual destructor */
  virtual ~IGsfSmoother() = default;

  /** Configure the GSF smoother
      - Configure the extrapolator
      - Configure the measurement updator */
  virtual StatusCode configureTools(
    const ToolHandle<IMultiStateExtrapolator>& extrapolator) = 0;

  /** Gsf smoother method */
  virtual SmoothedTrajectory* fit(
    const EventContext& ctx,
    Trk::IMultiStateExtrapolator::Cache&,
    const ForwardTrajectory&,
    const ParticleHypothesis particleHypothesis = nonInteracting,
    const CaloCluster_OnTrack* ccot = nullptr) const = 0;
};

} // end Trk namespace

#endif
