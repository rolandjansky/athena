/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  IGSFMaterialEffects_H
#define  IGSFMaterialEffects_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"
#include <memory>

namespace Trk {

class Layer;
class MaterialProperties;
class MaterialEffectsOnTrack;  

/** Interface ID for IGSFMaterialEffects*/  
static const InterfaceID IID_IGSFMaterialEffects("IGSFMaterialEffects", 1, 0);

class IGSFMaterialEffects : virtual public IAlgTool {

public:
  /**Virtual destructor*/
  virtual ~IGSFMaterialEffects(){}

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_IGSFMaterialEffects; }

  /** Layer based material effects update - track parameters given by pointer */
  virtual  std::unique_ptr<TrackParameters> update(const TrackParameters* parameters,
                                                   const Layer& layer,
                                                   PropDirection direction = anyDirection,
                                                   ParticleHypothesis particleHypothesis = nonInteracting,
                                                   MaterialUpdateMode matmode = Trk::addNoise) const = 0;

  /** Material properties based effects update - track parameters are given by reference */
  virtual std::unique_ptr<TrackParameters> update(const TrackParameters&,
                                                  const MaterialProperties&,
                                                  double,
                                                  PropDirection direction = anyDirection,
                                                  ParticleHypothesis particleHypothesis = nonInteracting,
                                                  MaterialUpdateMode matmode = Trk::addNoise) const =0 ;
};
}

#endif
