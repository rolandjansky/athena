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
class Layer;

class GsfMaterialMixtureConvolution
  : public AthAlgTool
  , virtual public IMaterialMixtureConvolution
{

public:
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
  virtual MultiComponentState update(const MultiComponentState&,
                                     const Layer&,
                                     PropDirection direction = anyDirection,
                                     ParticleHypothesis particleHypothesis =
                                       nonInteracting) const override final;

  //!< Convolution with pre-measurement-update material properties
  virtual MultiComponentState preUpdate(const MultiComponentState&,
                                        const Layer&,
                                        PropDirection direction = anyDirection,
                                        ParticleHypothesis particleHypothesis =
                                          nonInteracting) const override final;

  //!< Convolution with post-measurement-update material properties
  virtual MultiComponentState postUpdate(const MultiComponentState&,
                                         const Layer&,
                                         PropDirection direction = anyDirection,
                                         ParticleHypothesis particleHypothesis =
                                           nonInteracting) const override final;

  //!< Retain for now redundant simplified material effects
  virtual MultiComponentState simplifiedMaterialUpdate(
    const MultiComponentState& multiComponentState,
    PropDirection direction = anyDirection,
    ParticleHypothesis particleHypothesis =
      nonInteracting) const override final;

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
};

} // end Trk namespace

#endif
