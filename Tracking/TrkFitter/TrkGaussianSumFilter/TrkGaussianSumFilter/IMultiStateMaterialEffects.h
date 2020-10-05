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
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include <Eigen/StdVector>
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
    Cache()
    {
      weights.reserve(6);
      deltaPs.reserve(6);
      deltaParameters.reserve(6);
      deltaCovariances.reserve(6);
    }

    Cache(Cache&&) noexcept = default;
    Cache& operator=(Cache&&) noexcept = default;
    Cache(const Cache&) noexcept = default;
    Cache& operator=(const Cache&) noexcept = default;
    ~Cache() noexcept = default;

    std::vector<double> weights;
    std::vector<double> deltaPs;
    /*
     * Suggested
     * by Eigen 3.3.7 manual
     * "you must use the Eigen::aligned_allocator (not another aligned
     * allocator), and #include <Eigen/StdVector>."
     */
    std::vector<AmgVector(5), Eigen::aligned_allocator<AmgVector(5)>>
      deltaParameters;
    std::vector<AmgSymMatrix(5), Eigen::aligned_allocator<AmgSymMatrix(5)>>
      deltaCovariances;

    void reset()
    {
      weights.clear();
      deltaPs.clear();
      deltaParameters.clear();
      deltaCovariances.clear();
    }

    void resetAndAddDummyValues()
    {
      reset();
      weights.push_back(1);
      deltaPs.push_back(0);
      deltaParameters.emplace_back(AmgVector(5)::Zero());
      deltaCovariances.emplace_back(AmgSymMatrix(5)::Zero());
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
