/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   GsfCombinedMaterialEffects.h
 * @date   Friday 11th January 2005
 * @author Tom Athkinson, Anthony Morley, Christos Anastopoulos
 * Class definition for consideration of multiple scatter and energy
 * loss effects from material simultaneously
 */

#ifndef TrkGsfCombinedMaterialEffects_H
#define TrkGsfCombinedMaterialEffects_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkGaussianSumFilter/IBetheHeitlerEffects.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"

namespace Trk {
class GsfCombinedMaterialEffects
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffects
{
public:
  /** Constructor with AlgTool parameters*/
  GsfCombinedMaterialEffects(const std::string&,
                             const std::string&,
                             const IInterface*);

  /** Virtual destructor */
  virtual ~GsfCombinedMaterialEffects() override;

  /** AlgTool initialise method */
  virtual StatusCode initialize() override;

  /** AlgTool finalise method */
  StatusCode finalize() override;

  virtual void compute(
    IMultiStateMaterialEffects::Cache&,
    const ComponentParameters&,
    const MaterialProperties&,
    double,
    PropDirection = anyDirection,
    ParticleHypothesis = nonInteracting) const override final;

private:
  struct GSFScatteringCache
  {
    double deltaThetaCov = 0;
    double deltaPhiCov = 0;

    void reset()
    {
      deltaThetaCov = 0;
      deltaPhiCov = 0;
    }
  };

  void scattering(GSFScatteringCache&,
                  const ComponentParameters& componentParameters,
                  const MaterialProperties& materialProperties,
                  double pathLength) const;

  void energyLoss(Trk::GSFEnergyLossCache&,
                  const ComponentParameters&,
                  const MaterialProperties&,
                  double,
                  PropDirection direction = anyDirection,
                  ParticleHypothesis particleHypothesis = nonInteracting) const;

  ToolHandle<IMultipleScatteringUpdator> m_msUpdator{
    this,
    "MultipleScatteringUpdator",
    "Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator",
    ""
  };

  ToolHandle<IEnergyLossUpdator> m_EnergyLossUpdator{
    this,
    "EnergyLossUpdator",
    "Trk::EnergyLossUpdator/AtlasEnergyLossUpdator",
    ""
  };
  ToolHandle<IBetheHeitlerEffects> m_betheHeitlerEffects{
    this,
    "BetheHeitlerEffects",
    "Trk::GsfBetheHeitlerEffects/GsfBetheHeitlerEffects",
    ""
  };
};

} // end Trk namespace

#endif
