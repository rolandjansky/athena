/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMaterialEffectsUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IMATERIALEFFECTSUPDATOR_H
#define TRKEXINTERFACES_IMATERIALEFFECTSUPDATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkParameters/TrackParameters.h"
#include <memory>

namespace Trk {

class Layer;
class MaterialProperties;
class MaterialEffectsOnTrack;

/** Interface ID for IMaterialEffectsUpdator*/
static const InterfaceID IID_IMaterialEffectsUpdator("IMaterialEffectsUpdator",
                                                     1,
                                                     0);

/** @class IMaterialEffectsUpdator
  Interface class for the updater AlgTool, it inherits from IAlgTool

  Detailed information about private members and member functions can be found
  in the actual implementation class MaterialEffectsUpdator which inherits from
  this one.

  @author Andreas.Salzburger@cern.ch

  @author Christos Anastopoulos (Athena MT)
  */

class IMaterialEffectsUpdator : virtual public IAlgTool
{

public:
  /**Virtual destructor*/
  virtual ~IMaterialEffectsUpdator() {}

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID()
  {
    return IID_IMaterialEffectsUpdator;
  }

  /**
   * Abstract cache class to allow passing information to/between calls.
   * This can be particular useful in Athena MT
   * re-entrant algorithms
   */
  class ICache
  {
  public:
    /* we can make this concrete
     * if we do not need a Dummy
     * Material Effects updator
     */
    enum MaterialCacheType
    {
      MaterialEffects = 0,
      DummyMaterialEffects = 1
    };
    virtual MaterialCacheType type() const = 0;
    virtual ~ICache() = default;

  protected:
    ICache() = default;
  };
  /**
   * Creates an instance of the cache to be used.
   * by the client.
   */
  virtual std::unique_ptr<ICache> getCache() const = 0;

  /** Updator interface (full update for a layer):
    The parameters are given as a pointer owned by the caller.
    The returned ptr is owned by the caller.
    */
  virtual const TrackParameters* update(
    ICache& icache,
    const TrackParameters* param,
    const Layer& sf,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** User updator interface (full update for a layer):
    The parameters are given as a pointer owned by the caller.
    The returned ptr is owned by the caller
    */
  virtual TrackParameters* update(
    ICache& icache,
    const TrackParameters* param,
    const MaterialEffectsOnTrack& meff,
    Trk::ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** Updator interface (pre-update for a layer):
    The parameters are given as a pointer owned by the caller.
    The returned ptr is owned by the caller
    */
  virtual TrackParameters* preUpdate(
    ICache& icache,
    const TrackParameters* param,
    const Layer& sf,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** Updator interface (pre-update for a layer):
    The parameters are given as a pointer owned by the caller.
    The returned ptr is owned by the caller
    Layer-based material update if the postUpdate fails, it returns 0
    */
  virtual TrackParameters* postUpdate(
    ICache& icache,
    const TrackParameters& param,
    const Layer& sf,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** Updator interface:
    The parameters are given as a pointer owned by the caller.
    The returned ptr is owned by the caller
    - used by all Layer-based methods
    */
  virtual TrackParameters* update(
    ICache& icache,
    const TrackParameters& param,
    const MaterialProperties& mprop,
    double pathcorrection,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** Validation Action: */
  virtual void validationAction(ICache& icache) const = 0;

  /** Model Action:*/
  virtual void modelAction(ICache& icache,
                           const TrackParameters* param = nullptr) const = 0;

  /** Updator interface (full update for a layer):
    */
  virtual TrackParameters* update(
    const TrackParameters* param,
    const Layer& sf,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** User updator interface (full update for a layer):
    */
  virtual TrackParameters* update(
    const TrackParameters* param,
    const MaterialEffectsOnTrack& meff,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;
  /** Updator interface (pre-update for a layer):
    */
  virtual TrackParameters* preUpdate(
    const TrackParameters* param,
    const Layer& sf,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** Updator interface (pre-update for a layer):
    */
  virtual TrackParameters* postUpdate(
    const TrackParameters& param,
    const Layer& sf,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;

  /** Updator interface:
    MaterialProperties based material update
    - used by all Layer-based methods
    */
  virtual TrackParameters* update(
    const TrackParameters& param,
    const MaterialProperties& mprop,
    double pathcorrection,
    PropDirection dir = alongMomentum,
    ParticleHypothesis particle = pion,
    MaterialUpdateMode matupmode = addNoise
  ) const = 0;
  /** Validation Action:
    outside access to internal validation steps
    Optional */
  virtual void validationAction() const = 0;

  /** Model Action: Provides the possibility of
   * doing non-local MaterialEffectsUpdates for different models
   * Optional
   */
  virtual void modelAction(const TrackParameters* param = nullptr) const = 0;
};

} // end of namespace

#endif // TRKEXINTERFACES_MATERIALEFFECTSUPDATOR_H

