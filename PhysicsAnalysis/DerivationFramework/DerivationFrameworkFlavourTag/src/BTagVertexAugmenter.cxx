/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagVertexAugmenter.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"


BTagVertexAugmenter::BTagVertexAugmenter(
  const std::string& name, ISvcLocator* loc):
  AthAlgorithm(name, loc)
{
}

StatusCode BTagVertexAugmenter::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode BTagVertexAugmenter::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode BTagVertexAugmenter::execute() {
  const xAOD::VertexContainer *vertices = 0;
  CHECK( evtStore()->retrieve(vertices, "PrimaryVertices") );
  int npv = 0;
  size_t indexPV_tmp = 0;
  xAOD::VertexContainer::const_iterator vtx_itr = vertices->begin();
  xAOD::VertexContainer::const_iterator vtx_end = vertices->end();
  int count = -1;
  for (; vtx_itr != vtx_end; ++vtx_itr) {
    count++;
    if ((*vtx_itr)->nTrackParticles() >= 2) {
      npv++;
      if ((*vtx_itr)->vertexType() == 1) {
        indexPV_tmp = count;
      }
    }
  }
  std::unique_ptr<int> pv_index_ptr(new int(indexPV_tmp));
  CHECK(evtStore()->record(std::move(pv_index_ptr), "BTaggingVertexIndex"));
  std::unique_ptr<int> npv_ptr(new int(npv));
  CHECK(evtStore()->record(std::move(npv_ptr),
                           "BTaggingNumberOfPrimaryVertices"));
  return StatusCode::SUCCESS;
}
