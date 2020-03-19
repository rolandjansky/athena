/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolHandle.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"

namespace Trk {

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
  ToolHandle<IMultiStateMaterialEffects> m_multipleScatterEffects{
    this,
    "MultipleScatteringEffects",
    "Trk::MultipleScatteringEffects/MultipleScatterEffects",
    ""
  };
  ToolHandle<IMultiStateMaterialEffects> m_energyLossEffects{ this,
                                                              "EnergyLossEffects",
                                                              "Trk::GsfEnergyLossEffects/GsfEnergyLossEffects",
                                                              "" };
  ToolHandle<IMultiStateMaterialEffects> m_betheHeitlerEffects{ this,
                                                                "BetheHeitlerEffects",
                                                                "Trk::GsfBetheHeitlerEffects/GsfBetheHeitlerEffects",
                                                                "" };
};

} // end Trk namespace

#endif
