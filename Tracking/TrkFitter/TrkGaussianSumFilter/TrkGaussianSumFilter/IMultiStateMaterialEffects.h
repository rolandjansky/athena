/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
      IMultiStateMaterialEffects.h  -  description
      --------------------------------------------
begin                : Thursday 17th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : (Non-pure) abstract base class for defining material
                       effects including energy loss and multiple scattering for
                       use in the multi-component state environment. These
                       material effects will produce multi-component state outputs
************************************************************************************/

#ifndef Trk_IMultiStateMaterialEffects_H
#define Trk_IMultiStateMaterialEffects_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


#include "TrkExInterfaces/IMaterialEffectsUpdator.h"

namespace Trk {

class MaterialProperties;
//class IMaterialEffectsUpdator;

static const InterfaceID IID_IMultiStateMaterialEffects("IMultiStateMaterialEffects", 1, 0);



class IMultiStateMaterialEffects : virtual public IAlgTool{

 public:

  struct Cache{
    std::vector<double> weights;
    std::vector<double> deltaPs;
    std::vector<const AmgSymMatrix(5)*> deltaCovariances;
    ~Cache(){ reset(); };
    void reset();   
  };

  /** Alg tool and IAlgTool interface method */
  static const InterfaceID& interfaceID() { return IID_IMultiStateMaterialEffects; };

  /** virtual destructor */
  virtual ~IMultiStateMaterialEffects() {};

  virtual void compute( IMultiStateMaterialEffects::Cache&,
                        const ComponentParameters&,
                        const MaterialProperties&,
                        double,
                        PropDirection direction = anyDirection,
                        ParticleHypothesis particleHypothesis = nonInteracting ) const = 0;

};



}

#endif
