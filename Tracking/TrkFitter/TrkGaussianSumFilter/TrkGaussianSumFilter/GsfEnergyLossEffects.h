/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfEnergyLossEffects.h  -  description
      --------------------------------------
begin                : Tuesday 22nd January 2005
author               : amorley
email                : amorley@cern.ch
decription           : Class for describing energy loss effects only.
*********************************************************************************/

#ifndef TrkGsfEnergyLossEffects_H
#define TrkGsfEnergyLossEffects_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class Layer;
class MaterialProperties;

class GsfEnergyLossEffects
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffects
{

public:
  /** Constructor with AlgTool parameters */
  GsfEnergyLossEffects(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GsfEnergyLossEffects();

  /** AlgTool initialise method */
  StatusCode initialize() override final;

  /** AlgTool finalise method */
  StatusCode finalize() override final;

  virtual void compute(IMultiStateMaterialEffects::Cache&,
                       const ComponentParameters&,
                       const MaterialProperties&,
                       double,
                       PropDirection direction = anyDirection,
                       ParticleHypothesis particleHypothesis = nonInteracting) const override final;

private:
  ToolHandle<IEnergyLossUpdator> m_EnergyLossUpdator{ this,
                                                      "EnergyLossUpdator",
                                                      "Trk::EnergyLossUpdator/AtlasEnergyLossUpdator",
                                                      "" };
};

}

#endif
