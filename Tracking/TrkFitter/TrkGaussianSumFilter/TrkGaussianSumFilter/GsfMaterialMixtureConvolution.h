/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk{

class IMultiStateMaterialEffectsUpdator;
class IMultiComponentStateAssembler;
class IMultiComponentStateCombiner;
class MultiComponentState;
class Layer;

class GsfMaterialMixtureConvolution : public AthAlgTool, virtual public IMaterialMixtureConvolution {

 public:

  //!< Constructor with AlgTool parameters
  GsfMaterialMixtureConvolution( const std::string&, const std::string&, const IInterface* );

  //!< Destructor
  ~GsfMaterialMixtureConvolution();

  //!< AlgTool initialise method
  StatusCode initialize();

  //!< AlgTool finalize method
  StatusCode finalize();

  //!< Convolution with full material properties
  virtual const MultiComponentState* update( const MultiComponentState&,
					     const Layer&,
					     PropDirection direction = anyDirection,
					     ParticleHypothesis particleHypothesis = nonInteracting ) const;
  
  //!< Convolution with pre-measurement-update material properties
  virtual const MultiComponentState* preUpdate( const MultiComponentState&,
						const Layer&,
						PropDirection direction = anyDirection,
						ParticleHypothesis particleHypothesis = nonInteracting ) const;

  //!< Convolution with post-measurement-update material properties
  virtual const MultiComponentState* postUpdate( const MultiComponentState&,
						 const Layer&,
						 PropDirection direction = anyDirection,
						 ParticleHypothesis particleHypothesis = nonInteracting ) const;

  //!< Retain for now redundant simplified material effects
  virtual const MultiComponentState* simpliedMaterialUpdate( const MultiComponentState& multiComponentState,
							     PropDirection direction = anyDirection,
							     ParticleHypothesis particleHypothesis = nonInteracting ) const;

 private:
 
  int                                m_outputlevel;                      //!< to cache current output level
  
  ToolHandle<IMultiStateMaterialEffectsUpdator>  m_updator;            //!< Material effects updator

  ToolHandle<IMultiComponentStateCombiner>       m_stateCombiner;      //!< State combiner
  ToolHandle<IMultiComponentStateAssembler>      m_stateAssembler;     //!< State assembler

};

} // end Trk namespace

#endif
