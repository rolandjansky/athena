/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

///////////////////////////////////////////////////////////////////
// DummyMaterialEffectsUpdator.h, c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_DUMMYMATERIALEFFECTSUPDATOR_H
#define TRKEXTOOLS_DUMMYMATERIALEFFECTSUPDATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
// Trk
#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
class Layer;
//class TrackParameters;
class MaterialProperties;
class IMaterialMapper;

/** @class DummyMaterialEffectsUpdator

  Debug tool for navigation tests

  @author Andreas.Salzburger@cern.ch
  */
class DummyMaterialEffectsUpdator : public AthAlgTool,
  virtual public IMaterialEffectsUpdator {
  public:

    /** AlgTool like constructor */
    DummyMaterialEffectsUpdator(const std::string&,const std::string&,const IInterface*);

    /**Virtual destructor*/
    virtual ~DummyMaterialEffectsUpdator();

    /** AlgTool initailize method.*/
    StatusCode initialize() override;
    /** AlgTool finalize method */
    StatusCode finalize() override;

    /** Updator interface (full update for a layer): Dummy full update
    */
    std::unique_ptr<TrackParameters> update(
      const TrackParameters* parm,
      const Layer& sf,
      PropDirection dir = alongMomentum,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override;

    /** User updator interface (full update for a layer):
      The parmeters are given as a pointer, they are deleted inside the update method.
      Update occurs on the place where the parameters parm are according to the specified MaterialEffectsOnTrack
      */
    virtual std::unique_ptr<TrackParameters> update(
      const TrackParameters* parm,
      const MaterialEffectsOnTrack& meff,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override {
      (void)meff;
      (void)particle;
      (void)matupmode;
      return std::unique_ptr<TrackParameters>(parm->clone());
    }

    /** Updator interface (pre-update for a layer): Dummy pre update
    */
    virtual std::unique_ptr<TrackParameters> preUpdate(
      const TrackParameters* parm,
      const Layer& sf,
      PropDirection dir = alongMomentum,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override;

    /** Updator interface (post-update for a layer): Dummy post update
    */
    virtual std::unique_ptr<TrackParameters> postUpdate(
      const TrackParameters& parm,
      const Layer& sf,
      PropDirection dir = alongMomentum,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override;

    /** Updator interface:
      The parmeters are given as a pointer, they are delete inside the update method.
      MaterialProperties based material update
      - used by all Layer-based methods
      */
    virtual std::unique_ptr<TrackParameters> update(
      const TrackParameters& parm,
      const MaterialProperties& mprop,
      double pathcorrection,
      PropDirection dir = alongMomentum,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override;

    virtual void validationAction() const override {};

    virtual void modelAction(const TrackParameters* parm=nullptr) const override{
      if(parm) return;
    }

    typedef IMaterialEffectsUpdator::ICache ICache;
    class Cache : public ICache
    {
    public:
      virtual MaterialCacheType type() const override final
      {
        return ICache::DummyMaterialEffects;
      }
    };

    virtual std::unique_ptr<ICache> getCache() const override{
      return std::make_unique<Cache>();
    }

    virtual std::unique_ptr<TrackParameters> update(
      ICache& icache,
      const TrackParameters* parm,
      const Layer& sf,
      PropDirection dir = alongMomentum,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override {
      (void)icache;
      return update(parm, sf, dir, particle, matupmode);
    }

    virtual std::unique_ptr<TrackParameters> update(
      ICache& icache,
      const TrackParameters* parm,
      const MaterialEffectsOnTrack& meff,
      Trk::ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override {
      (void)icache;
      return update(parm, meff, particle, matupmode);
    }

    virtual std::unique_ptr<TrackParameters> preUpdate(
      ICache& icache,
      const TrackParameters* parm,
      const Layer& sf,
      PropDirection dir = alongMomentum,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override {
      (void)icache;
      return preUpdate(parm, sf, dir, particle, matupmode);
    }

    virtual std::unique_ptr<TrackParameters> postUpdate(
      ICache& icache,
      const TrackParameters& parm,
      const Layer& sf,
      PropDirection dir = alongMomentum,
      ParticleHypothesis particle = pion,
      MaterialUpdateMode matupmode = addNoise
    ) const override {
      (void)icache;
      return postUpdate(parm,sf,dir,particle,matupmode);
    }

    virtual std::unique_ptr<TrackParameters> update(
      ICache& icache, const TrackParameters& parm,
      const MaterialProperties& mprop,
      double pathcorrection,
      PropDirection dir=alongMomentum,
      ParticleHypothesis particle=pion,
      MaterialUpdateMode matupmode=addNoise
    ) const override {
      (void) icache;
      return update(parm,mprop,pathcorrection,dir,particle,matupmode);
    }

    /** Validation Action: */
    virtual void validationAction(ICache& icache) const override {
      (void) icache;
      validationAction();
    }

    /** Model Action:*/
    virtual void modelAction(ICache& icache,const TrackParameters* parm=nullptr) const override{
      (void) icache;
      modelAction(parm);
    }


  private:

    // ---------------- validation mode ------------------
    bool                         m_validationMode;            //!< boolean switch for the validation mode
    int                          m_validationDirectionSwitch; //!< jO switch for PropDirection
    PropDirection                m_validationDirection;       //!< distinction between forward and backward validation
    ToolHandle< IMaterialMapper > m_materialMapper;            //!< the material mapper for recording the layer material
  };

} // end of namespace


#endif // TRKEXTOOLS_DUMMYMATERIALEFFECTSUPDATOR_H

