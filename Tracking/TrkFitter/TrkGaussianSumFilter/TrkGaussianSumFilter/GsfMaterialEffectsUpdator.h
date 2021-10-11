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

#include "TrkGaussianSumFilter/IMultiStateMaterialEffectsUpdator.h"
#include "TrkGaussianSumFilterUtils/GsfCombinedMaterialEffects.h"
//
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

private:
  GsfCombinedMaterialEffects m_materialEffects{};

  Gaudi::Property<bool> m_useReferenceMaterial{ this,
                                                "UseReferenceMaterial",
                                                false,
                                                "" };

  Gaudi::Property<double> m_momentumCut{ this,
                                         "MinimalMomentum",
                                         250. * Gaudi::Units::MeV,
                                         "" };

  Gaudi::Property<std::string> m_parameterisationFileName{
    this,
    "BetheHeitlerParameterisationFileName",
    "GeantSim_LT01_cdf_nC6_O5.par",
    "Parametrization of Bethe Heitler material effects"
  };

  Gaudi::Property<std::string> m_parameterisationFileNameHighX0{
    this,
    "BetheHeitlerParameterisationFileNameHighX0",
    "GeantSim_GT01_cdf_nC6_O5.par",
    "Parametrization of Bethe Heitler material effects for high X0"
  };
};

} // end Trk namespace

#endif
