/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IBetheHeitlerEffects.h
 * @date   Thursday 8 September 2020
 * @author Anthony Morley, Christos Anastopoulos
 *
 * Abstract base class for defining material
 * effects following Bethe-Heitler formual
 */

#ifndef Trk_IBetheHeitlerEffects_H
#define Trk_IBetheHeitlerEffects_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include <Eigen/StdVector>
#include <memory>

namespace Trk {
struct GSFEnergyLossCache
{
  GSFEnergyLossCache()
  {
    weights.reserve(6);
    deltaPs.reserve(6);
    deltaQOvePCov.reserve(6);
  }

  GSFEnergyLossCache(GSFEnergyLossCache&&) noexcept = default;
  GSFEnergyLossCache& operator=(GSFEnergyLossCache&&) noexcept = default;
  GSFEnergyLossCache(const GSFEnergyLossCache&) noexcept = default;
  GSFEnergyLossCache& operator=(const GSFEnergyLossCache&) noexcept = default;
  ~GSFEnergyLossCache() noexcept = default;

  std::vector<double> weights;
  std::vector<double> deltaPs;
  std::vector<double> deltaQOvePCov;

  void reset()
  {
    weights.clear();
    deltaPs.clear();
    deltaQOvePCov.clear();
  }

  void resetAndAddDummyValues()
  {
    reset();
    weights.push_back(1);
    deltaPs.push_back(0);
    deltaQOvePCov.push_back(0);
  }
};

class MaterialProperties;

static const InterfaceID IID_IBetheHeitlerEffects("IBetheHeitlerEffects", 1, 0);

class IBetheHeitlerEffects : virtual public IAlgTool
{

public:
  /** Alg tool and IAlgTool interface method */
  static const InterfaceID& interfaceID() { return IID_IBetheHeitlerEffects; };

  /** virtual destructor */
  virtual ~IBetheHeitlerEffects() = default;

  virtual void compute(
    Trk::GSFEnergyLossCache& cache,
    const ComponentParameters& componentParameters,
    const MaterialProperties& materialProperties,
    double pathLength,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;
};
}
#endif
