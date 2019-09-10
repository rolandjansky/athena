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
#include "TrkGaussianSumFilter/IGSFMaterialEffects.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {

class Layer;
class MaterialProperties;

class GsfEnergyLossUpdator
  : public AthAlgTool
  , virtual public IGSFMaterialEffects
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

  /** Layer based material effects update - track parameters given by pointer */
  virtual std::unique_ptr<TrackParameters> update(const TrackParameters* parameters,
                                                  const Layer& layer,
                                                  PropDirection direction = anyDirection,
                                                  ParticleHypothesis particleHypothesis = nonInteracting,
                                                  MaterialUpdateMode matmode = Trk::addNoise) const override final;

  /** Material properties based effects update - track parameters are given by reference */
  virtual std::unique_ptr<TrackParameters> update(const TrackParameters&,
                                                  const MaterialProperties&,
                                                  double,
                                                  PropDirection direction = anyDirection,
                                                  ParticleHypothesis particleHypothesis = nonInteracting,
                                                  MaterialUpdateMode matmode = Trk::addNoise) const override final;

private:
  ToolHandle<IEnergyLossUpdator> m_EnergyLossUpdator{ this,
                                                      "EnergyLossUpdator",
                                                      "Trk::EnergyLossUpdator/AtlasEnergyLossUpdator",
                                                      "" };
};

}

#endif
