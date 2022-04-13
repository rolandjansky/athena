/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */
#include "TrkSurfaces/Surface.h"
#include "TrkExUtils/ExtrapolationCache.h" 
#include "TrkExTools/LocalExtrapolatorCache.h"


namespace Trk{

  Cache::Cache()
      : m_trackParmContainer(128)
      , // always reserve some space; still occasionally more slots are
        // needed; above 150 there are very few cases the max in q431 was 257
      m_lastValidParameters(m_trackParmContainer)
      , m_parametersAtBoundary(m_trackParmContainer)
    {
      m_navigSurfs.reserve(1024);
      m_navigVols.reserve(64);
      m_navigVolsInt.reserve(64);
    }
    
  Cache::Cache(const std::vector<const IMaterialEffectsUpdator*> & updaters)
  : m_trackParmContainer(128), m_lastValidParameters(m_trackParmContainer),
   m_parametersAtBoundary(m_trackParmContainer){
    m_navigSurfs.reserve(1024);
    m_navigVols.reserve(64);
    m_navigVolsInt.reserve(64);
    populateMatEffUpdatorCache(updaters);
  }
    
    Cache::~Cache()
    {
      s_navigSurfsMax.update(m_navigSurfs.size());
      s_navigVolsMax.update(m_navigVols.size());
      s_navigVolsIntMax.update(m_navigVols.size());
      if (m_ownParametersOnDetElements && m_parametersOnDetElements) {
        for (const Trk::TrackParameters* parm : *m_parametersOnDetElements) {
          delete parm;
        }
      }
      s_containerSizeMax.update(trackParmContainer().size());
    }



  IMaterialEffectsUpdator::ICache&
  Cache::subMaterialEffectsUpdatorCache( const TrackingVolume& tvol){
    return m_MaterialUpCache[tvol.geometrySignature()];
  }
  
  IMaterialEffectsUpdator::ICache&
  Cache::subMaterialEffectsUpdatorCache() {
    return m_MaterialUpCache[m_currentStatic->geometrySignature()];
  }
  
  void
  Cache::populateMatEffUpdatorCache(const std::vector<const IMaterialEffectsUpdator*> & updaters) {
    size_t numUpdaters = updaters.size();
    m_MaterialUpCache.reserve(numUpdaters);
    for (const auto & thisUpdater : updaters) {
      m_MaterialUpCache.emplace_back(thisUpdater->getCache());
    }
  }
  
  void
  Cache::setRecallInformation(const Surface& rsf,const Layer& rlay,const TrackingVolume& rvol) {
    m_recall = true;
    m_recallSurface = &rsf;
    m_recallLayer = &rlay;
    m_recallTrackingVolume = &rvol;
  }
  
  void
  Cache::resetRecallInformation() {
    m_recall = false;
    m_recallSurface = nullptr;
    m_recallLayer = nullptr;
    m_recallTrackingVolume = nullptr;
  }
  
  std::string 
  Cache::to_string(const std::string& txt) const{
    std::string result;
    if (elossPointerOverwritten()) {
      result = " NO dumpCache: Eloss cache pointer overwritten "
                    + std::to_string(reinterpret_cast<std::uintptr_t>(m_cacheEloss)) + " from extrapolationCache "
                    + std::to_string(reinterpret_cast<std::uintptr_t>(m_extrapolationCache->eloss()));
    } else {
      result = txt + " X0 " +std::to_string(m_extrapolationCache->x0tot())  + " Eloss deltaE "
                      + std::to_string(m_extrapolationCache->eloss()->deltaE()) + " Eloss sigma "
                      +  std::to_string(m_extrapolationCache->eloss()->sigmaDeltaE()) + " meanIoni "
                      + std::to_string(m_extrapolationCache->eloss()->meanIoni()) + " sigmaIoni "
                      + std::to_string(m_extrapolationCache->eloss()->sigmaIoni()) + " meanRad "
                      + std::to_string(m_extrapolationCache->eloss()->meanRad()) + " sigmaRad "
                      + std::to_string(m_extrapolationCache->eloss()->sigmaRad());
    }
    return result;
  }
  
  std::string
  Cache::checkCache(const std::string& txt) const{
    std::string result;
    if (elossPointerOverwritten()) {
      result = txt + " PROBLEM Eloss cache pointer overwritten " + std::to_string(reinterpret_cast<std::uintptr_t>(m_cacheEloss))
                        + " from extrapolationCache " + std::to_string(reinterpret_cast<std::uintptr_t>(m_extrapolationCache->eloss()));
    }
    return result;
  }
  
  bool
  Cache::elossPointerOverwritten() const{
    return (m_cacheEloss != nullptr && m_cacheEloss != m_extrapolationCache->eloss());
  }



}