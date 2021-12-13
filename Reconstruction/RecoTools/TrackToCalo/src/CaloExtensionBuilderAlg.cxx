/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     CaloExtensionBuilderAlg
PACKAGE:  offline/Reconstruction/RecoTools/TrackToCalo/CaloExtensionBuilderAlg

AUTHORS:  Anastopoulos/Capriles
PURPOSE:  Performs Calo Extension for all selected tracks
 **********************************************************************/
#include "CaloExtensionBuilderAlg.h"
//
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxCandidate.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

// std includes
#include <algorithm>
#include <cmath>
#include <memory>

StatusCode
Trk::CaloExtensionBuilderAlg::initialize()
{
  ATH_CHECK(m_particleCaloExtensionTool.retrieve());
  ATH_CHECK(m_ParticleCacheKey.initialize());
  ATH_CHECK(m_TrkPartContainerKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode
Trk::CaloExtensionBuilderAlg::execute(const EventContext& ctx) const
{
  /*
   * We want to extrapolate everything bar the the TRT-alone
   */
  SG::ReadHandle<xAOD::TrackParticleContainer> tracks(m_TrkPartContainerKey,
                                                      ctx);
  if (!tracks.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve TrackParticle container: "
                  << m_TrkPartContainerKey.key());
    return StatusCode::FAILURE;
  }
  // creating and saving the calo extension collection
  SG::WriteHandle<CaloExtensionCollection> lastCache(m_ParticleCacheKey, ctx);
  ATH_CHECK(lastCache.record(std::make_unique<CaloExtensionCollection>()));

  // set everything to false
  const xAOD::TrackParticleContainer* ptrTracks = tracks.cptr();
  std::vector<bool> mask(ptrTracks->size(), false);
  // set to true if 500 MeV and above
  // or not TRT-alone
  for (const auto* track : *tracks) {
    if (track->pt() < 500) {
      continue;
    }
    // Cut out TRT alone, but do it for all else
    if (track->patternRecoInfo().test(xAOD::TRTStandalone)) {
      continue;
    }
    mask[track->index()] = true;
  }

  ATH_CHECK(m_particleCaloExtensionTool->caloExtensionCollection(
    ctx, *ptrTracks, mask, *lastCache));

  return StatusCode::SUCCESS;
}
