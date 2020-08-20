/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfSmoother.h
 * @date   Friday 25th February 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Class definition for the GSF smoother
 */

#ifndef TrkGsfSmoother_H
#define TrkGsfSmoother_H

#include "TrkGaussianSumFilter/IGsfSmoother.h"
#include "TrkGaussianSumFilter/GsfMeasurementUpdator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

namespace Trk {

class IMultiStateMeasurementUpdator;
class IMultiStateExtrapolator;
class CaloCluster_OnTrack;

class GsfSmoother
  : public AthAlgTool
  , virtual public IGsfSmoother
{

public:
  /** Constructor with AlgTool parameters */
  GsfSmoother(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GsfSmoother() = default;

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Configure the GSF smoother
      - Configure the extrapolator
      - Configure the measurement updator */
  virtual StatusCode configureTools(
    const ToolHandle<IMultiStateExtrapolator>& extrapolator);

  /** Gsf smoother method */
  virtual SmoothedTrajectory* fit(
    const EventContext& ctx,
    Trk::IMultiStateExtrapolator::Cache&,
    const ForwardTrajectory&,
    const ParticleHypothesis particleHypothesis = nonInteracting,
    const CaloCluster_OnTrack* ccot = nullptr) const;

private:
  /** Method for combining the forwards fitted state and the smoothed state */
  MultiComponentState combine(const MultiComponentState&,
                              const MultiComponentState&) const;

  /** Methof to add the CaloCluster onto the track */
  MultiComponentState addCCOT(
    const EventContext& ctx,
    const Trk::TrackStateOnSurface* currentState,
    const Trk::CaloCluster_OnTrack* ccot,
    Trk::SmoothedTrajectory* smoothedTrajectory) const;

private:
  GsfMeasurementUpdator m_updator;
  bool m_combineWithFitter;

  Gaudi::Property<unsigned int> m_maximumNumberOfComponents{
    this,
    "MaximumNumberOfComponents",
    12,
    "Maximum number of components"
  };

  /*
   * Special Tool Handle set by the configureTools
   */
  ToolHandle<IMultiStateExtrapolator> m_extrapolator;
};

} // end Trk namespace

#endif
