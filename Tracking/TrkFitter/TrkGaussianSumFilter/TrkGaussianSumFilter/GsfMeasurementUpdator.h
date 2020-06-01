/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfMeasurementUpdator.h
 * @date   Friday 25th February 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief  Class for performing updates on multi-component states for the
 * gaussian-sum filter.
 */

#ifndef TrkGsfMeasurementUpdator_H
#define TrkGsfMeasurementUpdator_H

#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"

#include "TrkGaussianSumFilter/IMultiStateMeasurementUpdator.h"
#include "TrkToolInterfaces/IUpdator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class LocalParameters;

class GsfMeasurementUpdator
  : public AthAlgTool
  , virtual public IMultiStateMeasurementUpdator
{

private:
  /** Private typedef for calling the correct updator member function depending
   * of direction of fitting */
  typedef Trk::TrackParameters* (Trk::IUpdator::*Updator)(
    const Trk::TrackParameters&,
    const LocalParameters&,
    const Amg::MatrixX&,
    FitQualityOnSurface*&)const;

public:
  /** Constructor with parameters to be passed to AlgTool */
  GsfMeasurementUpdator(const std::string&,
                        const std::string&,
                        const IInterface*);

  /** Virtual destructor */
  virtual ~GsfMeasurementUpdator() = default;

  /** AlgTool initialise method */
  StatusCode initialize() override;

  /** AlgTool finalise method */
  StatusCode finalize() override;

  /** Method for updating the multi-state with a new measurement */
  virtual MultiComponentState update(
    MultiComponentState&&,
    const MeasurementBase&) const override final;

  /** Method for updating the multi-state with a new measurement and calculate
   * the fit qaulity at the same time*/
  virtual MultiComponentState update(
    Trk::MultiComponentState&&,
    const Trk::MeasurementBase&,
    std::unique_ptr<FitQualityOnSurface>& fitQoS) const override final;

  /** Method for GSF smoother to calculate unbiased parameters of the
   * multi-component state */
  virtual MultiComponentState getUnbiasedTrackParameters(
    MultiComponentState&&,
    const MeasurementBase&) const override final;

  /** Method for determining the chi2 of the multi-component state and the
   * number of degrees of freedom */
  virtual const FitQualityOnSurface* fitQuality(
    const MultiComponentState&,
    const MeasurementBase&) const override;

private:
  MultiComponentState calculateFilterStep(MultiComponentState&&,
                                          const MeasurementBase&,
                                          const Updator) const;

  MultiComponentState calculateFilterStep(
    MultiComponentState&&,
    const MeasurementBase&,
    std::unique_ptr<FitQualityOnSurface>& fitQoS) const;

  bool invalidComponent(const Trk::TrackParameters* trackParameters) const;

  MultiComponentState rebuildState(
    Trk::MultiComponentState&& stateBeforeUpdate) const;

private:
  ToolHandle<IUpdator> m_updator{ this,
                                  "Updator",
                                  "Trk::KalmanUpdator/KalmanUpdator",
                                  "" };
};
}

#endif
