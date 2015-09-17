/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfCombinedMaterialEffects.h  -  description
      --------------------------------------------
begin                : Friday 11th January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class definition for consideration of multiple scatter and
                       energy loss effects from material simultaneously.
*********************************************************************************/

#ifndef TrkGsfCombinedMaterialEffects_H
#define TrkGsfCombinedMaterialEffects_H

#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkGaussianSumFilter/MultiStateMaterialEffects.h"

#include "GaudiKernel/ToolHandle.h"


namespace Trk{

class MultiStateMaterialEffectsAdapter;
class IMaterialEffectsUpdator;

class GsfCombinedMaterialEffects : public MultiStateMaterialEffects, virtual public IMultiStateMaterialEffects {
 public:

  /** Constructor with AlgTool parameters*/
  GsfCombinedMaterialEffects(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GsfCombinedMaterialEffects();

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  /** Reset method to clear the cache */
  virtual void reset() const;

 private:
  virtual void compute ( const ComponentParameters&,
                         const MaterialProperties&,
                         double,
                         PropDirection = anyDirection,
                         ParticleHypothesis = nonInteracting ) const;

  private:
  
  ToolHandle<IMaterialEffectsUpdator> m_multipleScatterEffects;

  ToolHandle<IMultiStateMaterialEffects> m_multiStateMultipleScatteringEffects;

  ToolHandle<IMaterialEffectsUpdator> m_energyLossEffects;

  ToolHandle<IMultiStateMaterialEffects> m_betheHeitlerEffects;

  ToolHandle<IMultiStateMaterialEffects> m_multiStateEnergyLossEffects;

  const MultiStateMaterialEffectsAdapter* m_multiStateMultipleScatteringAdapter;
  const MultiStateMaterialEffectsAdapter* m_multiStateEnergyLossAdapter;
  
};

} // end Trk namespace

#endif
