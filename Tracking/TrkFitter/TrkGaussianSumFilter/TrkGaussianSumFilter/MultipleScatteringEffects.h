/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      MultipleScatteringEffects.h  -  description
      ----------------------------------------
begin                : Tuesday 22nd January 2005
author               : amorley
email                : amorley at cern.ch
decription           : Class for describing multiple scattering effects only.
*********************************************************************************/

#ifndef Trk_MultipleScatteringEffects_H
#define Trk_MultipleScatteringEffects_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class Layer;
class MaterialProperties;

class MultipleScatteringEffects
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffects
{

public:
  /** Constructor with AlgTool parameters */
  MultipleScatteringEffects(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~MultipleScatteringEffects();

  /** AlgTool initialise method */
  StatusCode initialize() override;

  /** AlgTool finalise method */
  StatusCode finalize() override;

  virtual void compute(IMultiStateMaterialEffects::Cache&,
                       const ComponentParameters&,
                       const MaterialProperties&,
                       double,
                       PropDirection direction = anyDirection,
                       ParticleHypothesis particleHypothesis = nonInteracting) const override final;

private:
  ToolHandle<IMultipleScatteringUpdator> m_msUpdator{ this,
                                                      "MultipleScatteringUpdator",
                                                      "Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator",
                                                      "" }; //!< AlgoTool for MultipleScatterin effects

  bool m_multipleScatterLogTermOn;
};

}

#endif
