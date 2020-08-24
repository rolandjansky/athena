/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexWeightCalculators/SumPtVertexWeightCalculator.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

namespace Trk {

StatusCode
SumPtVertexWeightCalculator::initialize()
{
  return StatusCode::SUCCESS;
} // end of initialize method

StatusCode
SumPtVertexWeightCalculator::finalize()
{
  return StatusCode::SUCCESS;
}

// class constructor implementation
SumPtVertexWeightCalculator::SumPtVertexWeightCalculator(const std::string& t,
                                                         const std::string& n,
                                                         const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_doSumPt2Selection(true)
{
  declareProperty("DoSumPt2Selection", m_doSumPt2Selection);
  declareInterface<IVertexWeightCalculator>(this);
}

double
SumPtVertexWeightCalculator::estimateSignalCompatibility(
  const xAOD::Vertex& vertex) const
{
  double total_pt = 0;
  ATH_MSG_DEBUG("Estimating vertex sorting score from "
                << vertex.nTrackParticles() << " tracks at vertex.");
  for (const auto& elTrackParticle : vertex.trackParticleLinks()) {

    if (not elTrackParticle.isValid()) {
      ATH_MSG_WARNING(
        "No valid link to tracks in xAOD::Vertex object. Skipping track for "
        "signal compatibility (may be serious).");
      continue;
    }

    const Trk::Perigee& perigee =
      (*elTrackParticle.cptr())->perigeeParameters();
    if (m_doSumPt2Selection) {
      total_pt += std::pow(1. / perigee.parameters()[Trk::qOverP] *
                             sin(perigee.parameters()[Trk::theta]) / 1000.,
                           2);
    } else {
      total_pt += std::fabs(1. / perigee.parameters()[Trk::qOverP]) *
                  sin(perigee.parameters()[Trk::theta]) / 1000.;
    }
  }
  return total_pt;
}

} /// End!!!

