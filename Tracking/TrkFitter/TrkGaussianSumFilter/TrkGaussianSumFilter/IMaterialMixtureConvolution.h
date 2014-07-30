/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
			IMaterialMixtureConvolution.h  -  description
			----------------------------------------------
begin                : Thursday 7th September 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Abstract base class for convolution of material effects
************************************************************************************/

#ifndef TrkIMaterialMixtureConvolution_H
#define TrkIMaterialMixtureConvolution_H

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "GaudiKernel/IAlgTool.h"

namespace Trk{

class MultiComponentState;
class Layer;

static const InterfaceID IID_IMaterialMixtureConvolution( "IMaterialMixtureConvolution", 1, 0 );

class IMaterialMixtureConvolution : virtual public IAlgTool {

 public:

  //!< IAlgTool and AlgTool interface method
  static const InterfaceID interfaceID() { return IID_IMaterialMixtureConvolution; };

  //!< Virtual destructor
  virtual ~IMaterialMixtureConvolution(){};

  //!< Convolution with full material properties
  virtual const MultiComponentState* update( const MultiComponentState&,
					     const Layer&,
					     PropDirection direction = anyDirection,
					     ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;
  
  //!< Convolution with pre-measurement-update material properties
  virtual const MultiComponentState* preUpdate( const MultiComponentState&,
						const Layer&,
						PropDirection direction = anyDirection,
						ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;
  
  //!< Convolution with post-measurement-update material properties
  virtual const MultiComponentState* postUpdate( const MultiComponentState&,
						 const Layer&,
						 PropDirection direction = anyDirection,
						 ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;
  
  //!< Retain for now redundant simplified material effects
  virtual const MultiComponentState* simpliedMaterialUpdate( const MultiComponentState& multiComponentState,
							     PropDirection direction = anyDirection,
							     ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;
  
};

} // end Trk namespace

# endif
