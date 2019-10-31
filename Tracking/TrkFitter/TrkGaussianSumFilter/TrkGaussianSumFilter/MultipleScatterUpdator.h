/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      MultipleScatterUpdator.h  -  description
      ----------------------------------------
begin                : Tuesday 22nd January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class for describing multiple scattering effects only.
                       More or less duplication from Andi
*********************************************************************************/

#ifndef Trk_MultipleScatteringUpdator_H
#define Trk_MultipleScatteringUpdator_H

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

class MultipleScatterUpdator
  : public AthAlgTool
  , virtual public IMultiStateMaterialEffects
{

public:
  /** Constructor with AlgTool parameters */
  MultipleScatterUpdator(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~MultipleScatterUpdator();

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
