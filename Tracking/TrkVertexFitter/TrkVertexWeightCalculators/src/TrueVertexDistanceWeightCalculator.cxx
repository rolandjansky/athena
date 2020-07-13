/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexWeightCalculators/TrueVertexDistanceWeightCalculator.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

namespace Trk {

StatusCode
TrueVertexDistanceWeightCalculator::initialize()
{
  ATH_CHECK(m_mcEventCollectionKey.initialize());
  return StatusCode::SUCCESS;
} 

StatusCode
TrueVertexDistanceWeightCalculator::finalize()
{
  return StatusCode::SUCCESS;
}

// class constructor implementation
TrueVertexDistanceWeightCalculator::TrueVertexDistanceWeightCalculator(
  const std::string& t,
  const std::string& n,
  const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<IVertexWeightCalculator>(this);
}

double
TrueVertexDistanceWeightCalculator::estimateSignalCompatibility(
  const xAOD::Vertex& vertex) const
{
  return mEstimateSignalCompatibility(vertex.position());
}

double
TrueVertexDistanceWeightCalculator::mEstimateSignalCompatibility(
  const Amg::Vector3D& vtxPosition) const
{
  SG::ReadHandle<McEventCollection> mcColl(m_mcEventCollectionKey);

  if (!mcColl.isValid()) {
    ATH_MSG_WARNING("Could not retrieve McEventCollection "
                    << m_mcEventCollectionKey.key()
                    << " from StoreGate. Returning 0 distance.");
    return 0;
  }

  McEventCollection::const_iterator itr = mcColl->begin();
  const HepMC::GenEvent* myEvent = (*itr);
  if (!myEvent) {
    ATH_MSG_ERROR(" Cannot find truth event...  Returning 0 distance... ");
    return 0;
  }

  HepMC::GenEvent::vertex_const_iterator Vert = myEvent->vertices_begin();
  ATH_MSG_DEBUG("Resulting MC seed:  x: " << (*Vert)->position().x()
                                          << " y: " << (*Vert)->position().y()
                                          << " z: " << (*Vert)->position().z());
  double ztrue = (*Vert)->position().z();

  double z_vtx_pos = vtxPosition.z();

  double distance = fabs(z_vtx_pos - ztrue);

  ATH_MSG_DEBUG("Distance of pV from truth:" << distance);

  if (distance == 0.) {
    ATH_MSG_WARNING(" Suspicious 0 distance.");
    return 1. / 0.000001;
  }
  ATH_MSG_DEBUG(" Returning distance: " << 1. / distance);
  return 1. / distance;
}
} 

