/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     EMGSFCaloExtensionBuilder
PACKAGE:  offline/Reconstruction/egamma/egammaTrackTools/EMGSFCaloExtensionBuilder

AUTHORS:  Anastopoulos
PURPOSE:  Performs Calo Extension for all GSF tracks 
 **********************************************************************/
#include "EMGSFCaloExtensionBuilder.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
//
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//std includes
#include <algorithm>
#include <cmath>
#include <memory>

EMGSFCaloExtensionBuilder::EMGSFCaloExtensionBuilder(const std::string& name, 
                                                     ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
}

StatusCode EMGSFCaloExtensionBuilder::initialize() 
{
  ATH_CHECK(m_GSFPerigeeCacheKey.initialize());
  ATH_CHECK(m_GSFLastCacheKey.initialize());
  ATH_CHECK(m_GSFTrkPartContainerKey.initialize());
  if(m_perigeeParticleCaloExtensionTool.retrieve().isFailure()){
    ATH_MSG_ERROR("initialize: Cannot retrieve  " << m_perigeeParticleCaloExtensionTool);
    return StatusCode::FAILURE;
  }
  if(m_lastParticleCaloExtensionTool.retrieve().isFailure()){
    ATH_MSG_ERROR("initialize: Cannot retrieve " << m_lastParticleCaloExtensionTool);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}  

StatusCode EMGSFCaloExtensionBuilder::EMGSFCaloExtensionBuilder::finalize(){ 
  return StatusCode::SUCCESS;
}

StatusCode EMGSFCaloExtensionBuilder::execute()
{
  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_GSFTrkPartContainerKey);

  if(!tracks.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "<< m_GSFTrkPartContainerKey.key());
    return StatusCode::FAILURE;
  }

  SG::WriteHandle<CaloExtensionCollection> perigeeCache(m_GSFPerigeeCacheKey);
  ATH_CHECK(perigeeCache.record(std::make_unique<CaloExtensionCollection>()));

  SG::WriteHandle<CaloExtensionCollection> lastCache(m_GSFLastCacheKey); 
  ATH_CHECK(lastCache.record(std::make_unique<CaloExtensionCollection>()));

  const xAOD::TrackParticleContainer* ptrTracks=tracks.cptr();
  CaloExtensionCollection* ptrPerigee=perigeeCache.ptr();
  CaloExtensionCollection* ptrLast=lastCache.ptr();
  std::vector<bool> mask (ptrTracks->size(),true);
  for (auto trkIt : *ptrTracks)  {
    //If we have TRT-SA tracks we don't extrapolate
    if (xAOD::EgammaHelpers::numberOfSiHits(trkIt) < 4) {
      mask[trkIt->index()] = false;
    }
  }
  ATH_CHECK(m_perigeeParticleCaloExtensionTool->caloExtensionCollection(*ptrTracks,mask,*ptrPerigee)); 
  ATH_CHECK(m_lastParticleCaloExtensionTool->caloExtensionCollection(*ptrTracks,mask,*ptrLast));

  return StatusCode::SUCCESS;
}


