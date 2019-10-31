/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      GsfEnergyLossUpdator.h  -  description
      --------------------------------------
begin                : Tuesday 22nd January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class for describing energy loss effects only.
                       More or less duplication from Andi
*********************************************************************************/

#ifndef TrkGsfEnergyLossUpdator_H
#define TrkGsfEnergyLossUpdator_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class Layer;
class MaterialProperties;

class GsfEnergyLossUpdator
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffects
{

public:
  /** Constructor with AlgTool parameters */
  GsfEnergyLossUpdator(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~GsfEnergyLossUpdator();

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
