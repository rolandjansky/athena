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
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/ToolHandle.h"

namespace Trk {

class Layer;
class MaterialProperties;

class MultipleScatterUpdator
  : public AthAlgTool
  , virtual public IMaterialEffectsUpdator
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

  /** Layer based material effects update - track parameters given by pointer */
  virtual const TrackParameters* update(const TrackParameters* parameters,
                                        const Layer& layer,
                                        PropDirection direction = anyDirection,
                                        ParticleHypothesis particleHypothesis = nonInteracting,
                                        MaterialUpdateMode matmode = Trk::addNoise) const override;

  /** Material properties based effects update - track parameters are given by reference */
  virtual const TrackParameters* update(const TrackParameters&,
                                        const MaterialProperties&,
                                        double,
                                        PropDirection direction = anyDirection,
                                        ParticleHypothesis particleHypothesis = nonInteracting,
                                        MaterialUpdateMode matmode = Trk::addNoise) const override;

  /** User updator interface (full update for a layer):
  The parmeters are given as a pointer, they are deleted inside the update method.
  Update occurs on the place where the parameters parm are according to the specified MaterialEffectsOnTrack
  */
  virtual const TrackParameters* update(const TrackParameters* parm,
                                        const MaterialEffectsOnTrack&,
                                        ParticleHypothesis particle = pion,
                                        MaterialUpdateMode matupmode = addNoise) const override
  {
    (void)particle;
    (void)matupmode;
    return parm;
  }

  /** Pre-Update */
  virtual const TrackParameters* preUpdate(const TrackParameters*,
                                           const Layer&,
                                           PropDirection,
                                           ParticleHypothesis,
                                           MaterialUpdateMode) const override
  {
    return 0;
  };

  /** Post-Update */
  virtual const TrackParameters* postUpdate(const TrackParameters&,
                                            const Layer&,
                                            PropDirection,
                                            ParticleHypothesis,
                                            MaterialUpdateMode) const override
  {
    return 0;
  };

  virtual void validationAction() const override{};

  virtual void modelAction(const TrackParameters* parm = 0) const override
  {
    if (parm)
      return;
  }

  typedef IMaterialEffectsUpdator::ICache ICache;
  class Cache : public ICache
  {};

  virtual std::unique_ptr<ICache> getCache() const override { return std::make_unique<Cache>(); }

  virtual const TrackParameters* update(ICache& icache,
                                        const TrackParameters* parm,
                                        const Layer& sf,
                                        PropDirection dir = alongMomentum,
                                        ParticleHypothesis particle = pion,
                                        MaterialUpdateMode matupmode = addNoise) const override
  {

    (void)icache;
    return update(parm, sf, dir, particle, matupmode);
  }

  virtual const TrackParameters* update(ICache& icache,
                                        const TrackParameters* parm,
                                        const MaterialEffectsOnTrack& meff,
                                        Trk::ParticleHypothesis particle = pion,
                                        MaterialUpdateMode matupmode = addNoise) const override
  {

    (void)icache;
    return update(parm, meff, particle, matupmode);
  }

  virtual const TrackParameters* preUpdate(ICache& icache,
                                           const TrackParameters* parm,
                                           const Layer& sf,
                                           PropDirection dir = alongMomentum,
                                           ParticleHypothesis particle = pion,
                                           MaterialUpdateMode matupmode = addNoise) const override
  {

    (void)icache;
    return preUpdate(parm, sf, dir, particle, matupmode);
  }

  virtual const TrackParameters* postUpdate(ICache& icache,
                                            const TrackParameters& parm,
                                            const Layer& sf,
                                            PropDirection dir = alongMomentum,
                                            ParticleHypothesis particle = pion,
                                            MaterialUpdateMode matupmode = addNoise) const override
  {

    (void)icache;
    return postUpdate(parm, sf, dir, particle, matupmode);
  }

  virtual const TrackParameters* update(ICache& icache,
                                        const TrackParameters& parm,
                                        const MaterialProperties& mprop,
                                        double pathcorrection,
                                        PropDirection dir = alongMomentum,
                                        ParticleHypothesis particle = pion,
                                        MaterialUpdateMode matupmode = addNoise) const override
  {
    (void)icache;
    return update(parm, mprop, pathcorrection, dir, particle, matupmode);
  }

  /** Validation Action: */
  virtual void validationAction(ICache& icache) const override
  {
    (void)icache;
    validationAction();
  }

  /** Model Action:*/
  virtual void modelAction(ICache& icache, const TrackParameters* parm = 0) const override
  {
    (void)icache;
    modelAction(parm);
  }

private:
  int m_outputlevel; //!< to cache current output level
  ToolHandle<IMultipleScatteringUpdator> m_msUpdator{ this,
                                                      "MultipleScatteringUpdator",
                                                      "Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator",
                                                      "" }; //!< AlgoTool for MultipleScatterin effects

  bool m_multipleScatterLogTermOn;
};

}

#endif
