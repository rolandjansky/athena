/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IMultiStateMaterialEffects.h
 * @date   Thursday 17th February 2005
 * @author Tom Atkinson, Anthony Morley, Christos Anastopoulos
 *
 * Abstract base class for defining material
 * effects including energy loss and multiple scattering for
 * use in the multi-component state environment. These
 * material effects will produce multi-component state
 */

#ifndef Trk_IMultiStateMaterialEffects_H
#define Trk_IMultiStateMaterialEffects_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGaussianSumFilter/GsfConstants.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include <memory>

namespace Trk {

class MaterialProperties;

static const InterfaceID
  IID_IMultiStateMaterialEffects("IMultiStateMaterialEffects", 1, 0);

class IMultiStateMaterialEffects : virtual public IAlgTool
{

public:
  struct Cache
  {
    std::array<double, GSFConstants::maxNumberofBHComponents> weights = {};
    std::array<double, GSFConstants::maxNumberofBHComponents> deltaPs = {};
    alignas(32)
      std::array<AmgVector(5),
                 GSFConstants::maxNumberofBHComponents> deltaParameters = {};
    alignas(32)
      std::array<AmgSymMatrix(5),
                 GSFConstants::maxNumberofBHComponents> deltaCovariances = {};

    size_t numWeights = 0;
    size_t numDeltaPs = 0;
    size_t numDeltaParameters = 0;
    size_t numDeltaCovariance = 0;
    void reset()
    {
      numWeights = 0;
      numDeltaPs = 0;
      numDeltaParameters = 0;
      numDeltaCovariance = 0;
    }

    void resetAndAddDummyValues()
    {
      weights[0]=1;
      deltaPs[0]=0;
      deltaParameters[0]=AmgVector(5)::Zero();
      deltaCovariances[0]=AmgSymMatrix(5)::Zero();
      numWeights = 1;
      numDeltaPs = 1;
      numDeltaParameters = 1;
      numDeltaCovariance = 1;
    }
  };

  /** Alg tool and IAlgTool interface method */
  static const InterfaceID& interfaceID()
  {
    return IID_IMultiStateMaterialEffects;
  };

  /** virtual destructor */
  virtual ~IMultiStateMaterialEffects() = default;

  virtual void compute(
    IMultiStateMaterialEffects::Cache&,
    const ComponentParameters&,
    const MaterialProperties&,
    double pathLength,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;
};
}
#endif
