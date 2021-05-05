/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
//
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
//
#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
#include "TrkGaussianSumFilterUtils/GsfMaterial.h"

#include <memory>

namespace Trk {

class MaterialProperties;

static const InterfaceID
  IID_IMultiStateMaterialEffects("IMultiStateMaterialEffects", 1, 0);

class IMultiStateMaterialEffects : virtual public IAlgTool
{

public:
  /** Alg tool and IAlgTool interface method */
  static const InterfaceID& interfaceID()
  {
    return IID_IMultiStateMaterialEffects;
  };

  /** virtual destructor */
  virtual ~IMultiStateMaterialEffects() = default;

  virtual void compute(
    GsfMaterial::Combined&,
    const ComponentParameters&,
    const MaterialProperties&,
    double pathLength,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;
};
}
#endif
