/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        IMultiStateMeasurementUpdator.h  -  description
                        -----------------------------------------------
begin                : Friday 25th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Interface class for performing updates on multi-component
                       state objects
*********************************************************************************/

#ifndef TrkIMultiStateMeasurementUpdator_H
#define TrkIMultiStateMeasurementUpdator_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
#include <memory>
namespace Trk {

class MeasurementBase;

static const InterfaceID IID_IMultiStateMeasurementUpdator("IMultiStateMeasurementUpdator", 1, 0);

class IMultiStateMeasurementUpdator : virtual public IAlgTool
{

public:
  /** IAlgTool and AlgTool interface method */
  static const InterfaceID interfaceID() { return IID_IMultiStateMeasurementUpdator; };

  /** Virtual destructor */
  virtual ~IMultiStateMeasurementUpdator()= default;

  /** Method for forward filters to incorporate measurement updates */
  virtual std::unique_ptr<MultiComponentState> update(MultiComponentState&&, const MeasurementBase&) const = 0;

  /** Method for updating the multi-state with a new measurement and calculate the fit qaulity at the same time*/
  virtual std::unique_ptr<MultiComponentState> update(Trk::MultiComponentState&&,
                                                      const Trk::MeasurementBase&,
                                                      std::unique_ptr<FitQualityOnSurface>& fitQoS) const = 0;

  /** Method for reverse filters (smoothers) to determine the unbiased track parameters */
  virtual std::unique_ptr<MultiComponentState> getUnbiasedTrackParameters(MultiComponentState&&,
                                                                          const MeasurementBase&) const = 0;

  /** Method for determining the chi2 of the track and the number of degrees of freedom */
  virtual const FitQualityOnSurface* fitQuality(const MultiComponentState&, const MeasurementBase&) const = 0;
};

} // end Trk namespace

#endif
