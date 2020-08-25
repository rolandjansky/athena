/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfMaterialMixtureConvolution.h
 * @date   Thursday 7th September 2006
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief Class description for convolution of GSF material mixture
 */

#ifndef TrkGsfMaterialMixtureConvolution_H
#define TrkGsfMaterialMixtureConvolution_H

#include "TrkGaussianSumFilter/IMaterialMixtureConvolution.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class IMultiStateMaterialEffectsUpdator;
class IMultiStateMaterialEffects;
class Layer;
class MaterialProperties;

class GsfMaterialMixtureConvolution
  : public AthAlgTool
  , virtual public IMaterialMixtureConvolution
{

public:
  enum MaterialUpdateType
  {
    Normal = 0,
    Preupdate = 1,
    Postupdate = 2
  };

  //!< Constructor with AlgTool parameters
  GsfMaterialMixtureConvolution(const std::string&,
                                const std::string&,
                                const IInterface*);

  //!< Destructor
  virtual ~GsfMaterialMixtureConvolution();

  //!< AlgTool initialise method
  virtual StatusCode initialize() override;

  //!< AlgTool finalize method
  virtual StatusCode finalize() override;

  //!< Convolution with full material properties
  virtual MultiComponentState update(
    std::vector<Trk::IMultiStateMaterialEffects::Cache>&,
    const MultiComponentState&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  //!< Convolution with pre-measurement-update material properties
  virtual MultiComponentState preUpdate(
    std::vector<Trk::IMultiStateMaterialEffects::Cache>&,
    const MultiComponentState&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  //!< Convolution with post-measurement-update material properties
  virtual MultiComponentState postUpdate(
    std::vector<Trk::IMultiStateMaterialEffects::Cache>&,
    const MultiComponentState&,
    const Layer&,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

  //!< Retain for now redundant simplified material effects
  virtual MultiComponentState simplifiedMaterialUpdate(
    const MultiComponentState&,
    PropDirection,
    ParticleHypothesis) const override final
  {
    return {};
  };

private:
  Gaudi::Property<unsigned int> m_maximumNumberOfComponents{
    this,
    "MaximumNumberOfComponents",
    12,
    "Maximum number of components"
  };

  ToolHandle<IMultiStateMaterialEffectsUpdator> m_updator{
    this,
    "MaterialEffectsUpdator",
    "Trk::GsfMaterialEffectsUpdator/GsfMaterialEffectsUpdator",
    ""
  };

  ToolHandle<IMultiStateMaterialEffects> m_materialEffects{
    this,
    "MaterialEffects",
    "Trk::GsfCombinedMaterialEffects/GsfCombinedMaterialEffects",
    ""
  };

  Gaudi::Property<bool> m_useReferenceMaterial{ this,
                                                "UseReferenceMaterial",
                                                false,
                                                "" };

  Gaudi::Property<double> m_momentumCut{ this,
                                         "MinimalMomentum",
                                         250. * Gaudi::Units::MeV,
                                         "" };

  Trk::MultiComponentState update(
    std::vector<Trk::IMultiStateMaterialEffects::Cache>&,
    const Trk::MultiComponentState& inputState,
    const Trk::Layer& layer,
    Trk::PropDirection direction,
    Trk::ParticleHypothesis particleHypothesis,
    MaterialUpdateType updateType) const;

  bool updateP(double& qOverP, double deltaP) const;

  std::pair<const Trk::MaterialProperties*, double> getMaterialProperties(
    const Trk::TrackParameters* trackParameters,
    const Trk::Layer& layer) const;
};

} // end Trk namespace

#endif
