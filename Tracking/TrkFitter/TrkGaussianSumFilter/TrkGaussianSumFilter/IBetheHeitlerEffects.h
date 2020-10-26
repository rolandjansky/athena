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
#include "TrkGaussianSumFilter/GsfConstants.h"
#include "TrkMultiComponentStateOnSurface/ComponentParameters.h"
#include <array>

namespace Trk {
struct GSFEnergyLossCache
{
  struct element
  {
    double weight = 0;
    double deltaP = 0;
    double deltaQOvePCov = 0;
  };
  std::array<element, GSFConstants::maxNumberofBHComponents> elements = {};
  int numElements = 0;
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
