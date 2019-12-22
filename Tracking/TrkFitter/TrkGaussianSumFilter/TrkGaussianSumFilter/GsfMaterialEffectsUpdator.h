/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        GsfMaterialEffectsUpdator.h  -  description
                        -------------------------------------------
begin                : Wednesday 9th January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Material effects for the GsfExtrapolator. It is an Alg
                        Tool inheriting from IAlgTool
*********************************************************************************/

#ifndef TrkGsfMaterialEffectsUpdator_H
#define TrkGsfMaterialEffectsUpdator_H

#include "TrkGaussianSumFilter/IMultiStateMaterialEffectsUpdator.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class IMultiStateMaterialEffects;

class GsfMaterialEffectsUpdator
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffectsUpdator
{

public:
  /** Constructor with AlgTool parameters */
  GsfMaterialEffectsUpdator(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GsfMaterialEffectsUpdator();

  /** AlgTool initialisation*/
  StatusCode initialize();

  /** AlgTool finalisation*/
  StatusCode finalize();

  /** Method for updating the state with material effects provided by the layer object */
  virtual std::unique_ptr<Trk::SimpleMultiComponentState> updateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method for updating the state with material effects provided by a material properties object and a pathlength */
  virtual std::unique_ptr<Trk::SimpleMultiComponentState> updateState(
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method for the state with material effects provided by the layer object prior to propagation */
  virtual std::unique_ptr<Trk::SimpleMultiComponentState> preUpdateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method for the state with material effects provided by the layer object after propagation */
  virtual std::unique_ptr<Trk::SimpleMultiComponentState> postUpdateState(
    const ComponentParameters&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

private:
  /** Method to perform centralised calculation of updated state */
  std::unique_ptr<Trk::SimpleMultiComponentState>  compute(
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis = nonInteracting) const;

  /** Method to calculate the updated momentum based on material effects */
  bool updateP(AmgVector(5)&, double) const;

private:
  ToolHandle<IMultiStateMaterialEffects>
    m_materialEffects{ this, "MaterialEffects", "Trk::GsfCombinedMaterialEffects/GsfCombinedMaterialEffects", "" };
  bool m_useReferenceMaterial;
  double m_momentumCut;
};

} // end Trk namespace

#endif
