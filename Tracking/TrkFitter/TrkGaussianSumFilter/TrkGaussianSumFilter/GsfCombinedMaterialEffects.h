/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class IGSFMaterialEffects;

class GsfCombinedMaterialEffects
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffects
{
public:
  /** Constructor with AlgTool parameters*/
  GsfCombinedMaterialEffects(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GsfCombinedMaterialEffects();

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalise method */
  StatusCode finalize();

  virtual void compute(IMultiStateMaterialEffects::Cache&,
                       const ComponentParameters&,
                       const MaterialProperties&,
                       double,
                       PropDirection = anyDirection,
                       ParticleHypothesis = nonInteracting) const;

private:
  ToolHandle<IGSFMaterialEffects> m_multipleScatterEffects{ this,
                                                            "MultipleScatteringUpdator",
                                                            "Trk::MultipleScatterUpdator/MultipleScatterUpdator",
                                                                "" };
  ToolHandle<IGSFMaterialEffects> m_energyLossEffects{ this,
                                                      "EnergyLossUpdator",
                                                      "Trk::GsfEnergyLossUpdator/MyGsfEnergyLossUpdator",
                                                      "" };
  ToolHandle<IMultiStateMaterialEffects> m_betheHeitlerEffects{ this,
                                                                "BetheHeitlerEffects",
                                                                "Trk::GsfBetheHeitlerEffects/GsfBetheHeitlerEffects",
                                                                "" };
};

} // end Trk namespace

#endif
