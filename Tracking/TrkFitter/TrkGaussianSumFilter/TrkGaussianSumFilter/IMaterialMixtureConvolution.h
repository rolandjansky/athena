/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IMaterialMixtureConvolution.h
 * @date   Thursday 7th September 2006
 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Abstract base class for convolution of material effects
 */

#ifndef TrkIMaterialMixtureConvolution_H
#define TrkIMaterialMixtureConvolution_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

namespace Trk {
class Layer;
static const InterfaceID
  IID_IMaterialMixtureConvolution("IMaterialMixtureConvolution", 1, 0);

class IMaterialMixtureConvolution : virtual public IAlgTool
{

public:
  //!< IAlgTool and AlgTool interface method
  static const InterfaceID& interfaceID()
  {
    return IID_IMaterialMixtureConvolution;
  };

  //!< Virtual destructor
  virtual ~IMaterialMixtureConvolution() = default;

  //!< Convolution with full material properties
  virtual MultiComponentState update(
    std::vector<Trk::IMultiStateMaterialEffects::Cache>&,
    const MultiComponentState&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  //!< Convolution with pre-measurement-update material properties
  virtual MultiComponentState preUpdate(
    std::vector<Trk::IMultiStateMaterialEffects::Cache>&,
    const MultiComponentState&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  //!< Convolution with post-measurement-update material properties
  virtual MultiComponentState postUpdate(
    std::vector<Trk::IMultiStateMaterialEffects::Cache>&,
    const MultiComponentState&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  //!< Retain for now redundant simplified material effects
  virtual MultiComponentState simplifiedMaterialUpdate(
    const MultiComponentState& multiComponentState,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;
};

} // end Trk namespace

#endif
