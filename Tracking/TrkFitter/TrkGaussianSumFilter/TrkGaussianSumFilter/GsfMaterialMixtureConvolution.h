/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
                        GsfMaterialMixtureConvolution.h  -  description
                        -----------------------------------------------
begin                : Thursday 7th September 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class description for convolution of GSF material mixture
************************************************************************************/

#ifndef TrkGsfMaterialMixtureConvolution_H
#define TrkGsfMaterialMixtureConvolution_H

#include "TrkGaussianSumFilter/IMaterialMixtureConvolution.h"
#include "TrkGaussianSumFilter/IMultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/IMultiComponentStateMerger.h"
#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class IMultiStateMaterialEffectsUpdator;
class MultiComponentState;
class Layer;

class GsfMaterialMixtureConvolution
  : public AthAlgTool
  , virtual public IMaterialMixtureConvolution
{

public:
  //!< Constructor with AlgTool parameters
  GsfMaterialMixtureConvolution(const std::string&, const std::string&, const IInterface*);

  //!< Destructor
  virtual ~GsfMaterialMixtureConvolution();

  //!< AlgTool initialise method
  virtual StatusCode initialize() override;

  //!< AlgTool finalize method
  virtual StatusCode finalize() override;

  //!< Convolution with full material properties
  virtual std::unique_ptr<MultiComponentState> 
    update(const MultiComponentState&,
           const Layer&,
           PropDirection direction = anyDirection,
           ParticleHypothesis particleHypothesis = nonInteracting) const override final;

  //!< Convolution with pre-measurement-update material properties
  virtual std::unique_ptr<MultiComponentState> 
    preUpdate(const MultiComponentState&,
              const Layer&,
              PropDirection direction = anyDirection,
              ParticleHypothesis particleHypothesis = nonInteracting) const override final;

  //!< Convolution with post-measurement-update material properties
  virtual std::unique_ptr<MultiComponentState> 
    postUpdate(const MultiComponentState&,
               const Layer&,
               PropDirection direction = anyDirection,
               ParticleHypothesis particleHypothesis = nonInteracting) const override final;

  //!< Retain for now redundant simplified material effects
  virtual std::unique_ptr<MultiComponentState> 
    simpliedMaterialUpdate(const MultiComponentState& multiComponentState,
                           PropDirection direction = anyDirection,
                           ParticleHypothesis particleHypothesis = nonInteracting) const override final;

private:
  ToolHandle<IMultiStateMaterialEffectsUpdator> m_updator{ 
    this,
    "MaterialEffectsUpdator",
    "Trk::GsfMaterialEffectsUpdator/GsfMaterialEffectsUpdator",
    ""
  };
  ToolHandle<IMultiComponentStateCombiner> m_stateCombiner{
    this,
    "MultiComponentStateCombiner",
    "Trk::MultiComponentStateCombiner/MultiComponentStateCombiner",
    ""
  };
  ToolHandle<IMultiComponentStateAssembler> m_stateAssembler{
    this,
    "MultiComponentStateAssembler",
    "Trk::MultiComponentStateAssembler/MaterialConvolutionAssembler",
    ""
  };
  ToolHandle<IMultiComponentStateMerger> m_stateMerger{
    this,
    "MultiComponentStateMerger",
    "Trk::QuickCloseComponentsMultiStateMerger/MaterialConvolutionMerger",
    ""
  };

};

} // end Trk namespace

#endif
