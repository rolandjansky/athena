/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   ElectronMaterialMixtureConvolution.h
 * @date   Thursday 7th September 2006
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * @brief Class description for convolution of GSF material mixture
 */

#ifndef TrkElectronMaterialMixtureConvolution_H
#define TrkElectronMaterialMixtureConvolution_H

#include "TrkGaussianSumFilter/IMaterialMixtureConvolution.h"
//
#include "TrkGaussianSumFilterUtils/GsfMaterial.h"
#include "TrkGaussianSumFilterUtils/MultiComponentState.h"
#include "TrkGaussianSumFilterUtils/ElectronCombinedMaterialEffects.h"
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {
class Layer;
class MaterialProperties;

class ElectronMaterialMixtureConvolution final
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
  ElectronMaterialMixtureConvolution(const std::string&,
                                const std::string&,
                                const IInterface*);

  //!< Destructor
  virtual ~ElectronMaterialMixtureConvolution();

  //!< AlgTool initialise method
  virtual StatusCode initialize() override final;

  //!< Convolution with full material properties
  virtual MultiComponentState update(std::vector<GsfMaterial::Combined>&,
                                     const MultiComponentState&,
                                     const Layer&,
                                     PropDirection direction = anyDirection,
                                     ParticleHypothesis particleHypothesis =
                                       nonInteracting) const override final;

  //!< Convolution with pre-measurement-update material properties
  virtual MultiComponentState preUpdate(std::vector<GsfMaterial::Combined>&,
                                        const MultiComponentState&,
                                        const Layer&,
                                        PropDirection direction = anyDirection,
                                        ParticleHypothesis particleHypothesis =
                                          nonInteracting) const override final;

  //!< Convolution with post-measurement-update material properties
  virtual MultiComponentState postUpdate(std::vector<GsfMaterial::Combined>&,
                                         const MultiComponentState&,
                                         const Layer&,
                                         PropDirection direction = anyDirection,
                                         ParticleHypothesis particleHypothesis =
                                           nonInteracting) const override final;


private:
  Trk::MultiComponentState update(std::vector<GsfMaterial::Combined>&,
                                  const Trk::MultiComponentState& inputState,
                                  const Trk::Layer& layer,
                                  Trk::PropDirection direction,
                                  Trk::ParticleHypothesis particleHypothesis,
                                  MaterialUpdateType updateType) const;

  ElectronCombinedMaterialEffects m_materialEffects{};

  Gaudi::Property<unsigned int> m_maximumNumberOfComponents{
    this,
    "MaximumNumberOfComponents",
    12,
    "Maximum number of components"
  };

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
