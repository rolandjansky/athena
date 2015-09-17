/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
      IMultiStateMaterialEffects.h  -  description
      --------------------------------------------
begin                : Thursday 17th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : (Non-pure) abstract base class for defining material
                       effects including energy loss and multiple scattering for
                       use in the multi-component state environment. These
                       material effects will produce multi-component state outputs
************************************************************************************/

#ifndef Trk_IMultiStateMaterialEffects_H
#define Trk_IMultiStateMaterialEffects_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


#include "TrkExInterfaces/IMaterialEffectsUpdator.h"

namespace Trk {

class MaterialProperties;
//class IMaterialEffectsUpdator;

static const InterfaceID IID_IMultiStateMaterialEffects("IMultiStateMaterialEffects", 1, 0);
  

class IMultiStateMaterialEffects : virtual public IAlgTool{
 public:
  /** Alg tool and IAlgTool interface method */
  static const InterfaceID& interfaceID() { return IID_IMultiStateMaterialEffects; };
  
  /** virtual destructor */
  virtual ~IMultiStateMaterialEffects() {};
  
  /** Method to determine the weights of new components after material effects */
  virtual const std::vector<double> weights ( const ComponentParameters&,
                const MaterialProperties&,
                double,
                PropDirection direction = anyDirection,
                ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;

  /** Method to determine the change in momentum of new components after material effects */
  virtual const std::vector<double> deltaPs ( const ComponentParameters&,
                const MaterialProperties&,
                double,
                PropDirection direction = anyDirection,
                ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;

  /** Method to determine the change in covariance of new components after material effects */
  virtual const std::vector<const AmgSymMatrix(5)*> deltaCovariances ( const ComponentParameters&,
                   const MaterialProperties&,
                   double,
                   PropDirection direction = anyDirection,
                   ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;

  /** Method to allow interface between single and multi-component states (through an adapter) */
  virtual void setMaterialEffects(const ToolHandle<IMaterialEffectsUpdator> &) const = 0;

  virtual void reset() const = 0;

  virtual bool isReset() const = 0;

};
 
}

#endif
