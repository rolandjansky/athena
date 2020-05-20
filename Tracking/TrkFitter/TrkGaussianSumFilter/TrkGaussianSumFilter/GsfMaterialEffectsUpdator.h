/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfMaterialEffectsUpdator.h
 * @date   Wednesday 9th January 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief Material effects for the GsfExtrapolator.
 */

#ifndef TrkGsfMaterialEffectsUpdator_H
#define TrkGsfMaterialEffectsUpdator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffectsUpdator.h"

namespace Trk {

class IMultiStateMaterialEffects;

class GsfMaterialEffectsUpdator
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffectsUpdator
{

public:
  /** Constructor with AlgTool parameters */
  GsfMaterialEffectsUpdator(const std::string&,
                            const std::string&,
                            const IInterface*);

  /** Virtual destructor */
  virtual ~GsfMaterialEffectsUpdator();

  /** AlgTool initialisation*/
  StatusCode initialize();

  /** AlgTool finalisation*/
  StatusCode finalize();

  /** Method for updating the state with material effects provided by the layer
   * object */
  virtual Trk::MultiComponentState updateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method for updating the state with material effects provided by a material
   * properties object and a pathlength */
  virtual Trk::MultiComponentState updateState(
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method for the state with material effects provided by the layer object
   * prior to propagation
   */
  virtual Trk::MultiComponentState preUpdateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method for the state with material effects provided by the layer object
   * after propagation */
  virtual Trk::MultiComponentState postUpdateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

private:
  /** Method to perform centralised calculation of updated state */
  Trk::MultiComponentState compute(
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method to calculate the updated momentum based on material effects */
  bool updateP(AmgVector(5) &, double) const;

private:
  ToolHandle<IMultiStateMaterialEffects> m_materialEffects{
    this,
    "MaterialEffects",
    "Trk::GsfCombinedMaterialEffects/GsfCombinedMaterialEffects",
    ""
  };
  bool m_useReferenceMaterial;
  double m_momentumCut;
};

} // end Trk namespace

#endif
