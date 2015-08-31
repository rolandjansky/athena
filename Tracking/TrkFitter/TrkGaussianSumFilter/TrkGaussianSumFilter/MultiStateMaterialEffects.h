/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
      MultiStateMaterialEffects.h  -  description
      --------------------------------------------
begin                : Thursday 17th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : (Non-pure)  Base class for defining material
                       effects including energy loss and multiple scattering for
                       use in the multi-component state environment. These
                       material effects will produce multi-component state outputs
************************************************************************************/

#ifndef Trk_MultiStateMaterialEffects_H
#define Trk_MultiStateMaterialEffects_H

#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class MaterialProperties;
class IMaterialEffectsUpdator;
 

class MultiStateMaterialEffects : public AthAlgTool, virtual public IMultiStateMaterialEffects  
{
 public:
  
  /** Constructor with standard AlgTool parameters */
  MultiStateMaterialEffects( const std::string&, const std::string&, const IInterface* );

  /** virtual destructor */
  virtual ~MultiStateMaterialEffects() {};
  
  /** Method to determine the weights of new components after material effects */
  virtual const std::vector<double> weights ( const ComponentParameters&,
                const MaterialProperties&,
                double,
                PropDirection direction = anyDirection,
                ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Method to determine the change in momentum of new components after material effects */
  virtual const std::vector<double> deltaPs ( const ComponentParameters&,
                const MaterialProperties&,
                double,
                PropDirection direction = anyDirection,
                ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Method to determine the change in covariance of new components after material effects */
  virtual const std::vector<const AmgSymMatrix(5)*> deltaCovariances ( const ComponentParameters&,
                   const MaterialProperties&,
                   double,
                   PropDirection direction = anyDirection,
                   ParticleHypothesis particleHypothesis = nonInteracting ) const;

  /** Method to allow interface between single and multi-component states (through an adapter) */
  virtual void setMaterialEffects( const ToolHandle<IMaterialEffectsUpdator> &) const;

  virtual void reset() const;

 protected:
  bool isReset() const;

 private:
  /** Gsf Material effects calculation */
  virtual void compute ( const ComponentParameters&, 
       const MaterialProperties&,
       double,
       PropDirection direction = anyDirection,
       ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;
  
  
 protected:
  mutable bool m_isReset;

  mutable ToolHandle<IMaterialEffectsUpdator> m_materialEffectsUpdator;

  mutable std::vector<double> m_weights;
  mutable std::vector<double> m_deltaPs;
  mutable std::vector<const AmgSymMatrix(5)*> m_deltaCovariances;
};
 
}

inline void Trk::MultiStateMaterialEffects::setMaterialEffects(const ToolHandle<IMaterialEffectsUpdator> &iMaterialEffectsUpdator) const
{
	 m_materialEffectsUpdator = iMaterialEffectsUpdator;
}

inline bool Trk::MultiStateMaterialEffects::isReset() const
{

  return m_isReset;

}

#endif
