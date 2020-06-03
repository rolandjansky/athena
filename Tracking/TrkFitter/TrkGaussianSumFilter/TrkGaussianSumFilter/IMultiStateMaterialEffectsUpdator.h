/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IMultiStateMaterialEffectsUpdator.h
 * @date   Tuesday 1stMarch 2005
 * @author Tom Atkinson, Anthony Morley, Christos Anastopoulos
 * @brief Base class definition for material effects for multi-component states
 */

#ifndef TrkIMultiStateMaterialEffectsUpdator_H
#define TrkIMultiStateMaterialEffectsUpdator_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "GaudiKernel/IAlgTool.h"

namespace Trk {

class MaterialProperties;
class Layer;

static const InterfaceID IID_IMultiStateMaterialEffectsUpdator(
  "IMultiStateMaterialEffectsUpdator",
  1,
  0);

class IMultiStateMaterialEffectsUpdator : virtual public IAlgTool
{

public:
  /** AlgTool interface method */
  static const InterfaceID& interfaceID()
  {
    return IID_IMultiStateMaterialEffectsUpdator;
  };

  /** Virtual destructor */
  virtual ~IMultiStateMaterialEffectsUpdator() = default;

  /** Method for updating the state with material effects provided by the layer
   * object */
  virtual Trk::MultiComponentState updateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = alongMomentum,
    ParticleHypothesis = nonInteracting) const = 0;

  /** Method for updating the state with material effects provided by a material
   * properties object and a pathlength */
  virtual Trk::MultiComponentState updateState(
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection = alongMomentum,
    ParticleHypothesis = nonInteracting) const = 0;

  /** Method for the state with material effects provided by the layer object
   * prior to propagation
   */
  virtual Trk::MultiComponentState preUpdateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;

  /**  Method for the state with material effects provided by the layer object
   * after propagation */
  virtual Trk::MultiComponentState postUpdateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const = 0;
};

} // end Trk namespace

#endif
