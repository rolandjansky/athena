/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   ForwardGsfFitter.h
 * @date   Wednesday 9th March 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief   Class definition for the forward GSF fitter
 */

#ifndef TrkForwardGsfFitter_H
#define TrkForwardGsfFitter_H
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkGaussianSumFilter/GsfMeasurementUpdator.h"
#include "TrkGaussianSumFilter/IForwardGsfFitter.h"
#include "TrkGaussianSumFilter/IMultiStateExtrapolator.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkParameters/TrackParameters.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class IMultiStateMeasurementUpdator;
class IRIO_OnTrackCreator;
class Surface;

class ForwardGsfFitter
  : public AthAlgTool
  , virtual public IForwardGsfFitter
{

public:
  /** Constructor with AlgTool parameters */
  ForwardGsfFitter(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~ForwardGsfFitter() = default;

  /** AlgTool initialise method */
  virtual StatusCode initialize() override final;

  /** AlgTool finalise method */
  virtual StatusCode finalize() override final;

  /** Configure the forward GSF fitter
      - Configure the extrapolator
      - Configure the measurement updator
      - Configure the RIO_OnTrack creator */
  virtual StatusCode configureTools(
    const ToolHandle<Trk::IMultiStateExtrapolator>& extrapolator,
    const ToolHandle<Trk::IRIO_OnTrackCreator>& rioOnTrackCreator)
    override final;

  /** Forward GSF fit using PrepRawData */
  virtual std::unique_ptr<ForwardTrajectory> fitPRD(
    const EventContext& ctx,
    IMultiStateExtrapolator::Cache&,
    const PrepRawDataSet&,
    const TrackParameters&,
    const ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** Forward GSF fit using MeasurementSet */
  virtual std::unique_ptr<ForwardTrajectory> fitMeasurements(
    const EventContext& ctx,
    IMultiStateExtrapolator::Cache&,
    const MeasurementSet&,
    const TrackParameters&,
    const ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  /** The interface will later be extended so that the initial
   * state can be additionally a MultiComponentState object! */

private:
  /** Progress one step along the fit */
  bool stepForwardFit(
    const EventContext& ctx,
    IMultiStateExtrapolator::Cache&,
    ForwardTrajectory*,
    const PrepRawData*,
    const MeasurementBase*,
    const Surface&,
    MultiComponentState&,
    const ParticleHypothesis particleHypothesis = nonInteracting) const;

private:
  /**These are passed via the configure tools so not retrieved from this tool*/
  ToolHandle<IMultiStateExtrapolator> m_extrapolator;
  ToolHandle<IRIO_OnTrackCreator> m_rioOnTrackCreator;
  GsfMeasurementUpdator m_updator;
  double m_cutChiSquaredPerNumberDOF;
  int m_overideMaterialEffects;
  ParticleHypothesis m_overideParticleHypothesis;
  bool m_overideMaterialEffectsSwitch;
};

} // end Trk namespace

#endif
