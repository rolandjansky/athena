/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkVertexWeightCalculators/VxProbVertexWeightCalculator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "VxVertex/VxTrackAtVertex.h"
#include <TH1.h>
#include <TMath.h>

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"

namespace Trk {

StatusCode
VxProbVertexWeightCalculator::initialize()
{
  ATH_CHECK(service("THistSvc", m_iTHistSvc, true));
  ATH_CHECK(m_iTHistSvc->regHist(m_HistoFileLocation));
  ATH_CHECK(m_iTHistSvc->getHist(m_HistoFileLocation, m_hMinBiasPt));
  ATH_MSG_DEBUG("Initialization successful");
  return StatusCode::SUCCESS;
} // end of initialize method

StatusCode
VxProbVertexWeightCalculator::finalize()
{
  ATH_MSG_VERBOSE("Finalize successful");
  return StatusCode::SUCCESS;
}

// class constructor implementation
VxProbVertexWeightCalculator::VxProbVertexWeightCalculator(const std::string& t,
                                                           const std::string& n,
                                                           const IInterface* p)
  : AthAlgTool(t, n, p)
  , m_hMinBiasPt(nullptr)
  , m_HistoFileLocation("/VxProbHisto/h_sTrkPdfminBias")
  , m_iTHistSvc()
{
  declareProperty("HistogramPath", m_HistoFileLocation);
  declareInterface<IVertexWeightCalculator>(this);
}


double
VxProbVertexWeightCalculator::estimateSignalCompatibility(
  const xAOD::Vertex& vertex) const
{
  double P0 = 1.;
  // bool pTtooBig = false;
  for (const auto& elTrackParticle : vertex.trackParticleLinks()) {
    if (not elTrackParticle.isValid()) {
      ATH_MSG_WARNING(
        "No valid link to tracks in xAOD::Vertex object. Skipping track for "
        "signal compatibility (may be serious).");
      continue;
    }
    const Trk::Perigee& perigee =
      (*elTrackParticle.cptr())->perigeeParameters();
    double p_T = std::fabs(1. / perigee.parameters()[Trk::qOverP]) *
                 sin(perigee.parameters()[Trk::theta]) / 1000.;
    TH1F* myHisto = dynamic_cast<TH1F*>(m_hMinBiasPt);
    if (myHisto == nullptr) {
      ATH_MSG_WARNING("VxProbHisto is an empty pointer. Signal compatibility "
                      "failed and returning 0.0");
      return 0;
    }
    double IntPt =
      (myHisto->Integral(myHisto->FindBin(p_T), myHisto->GetNbinsX() + 1) +
       myHisto->Integral(myHisto->FindBin(p_T) + 1, myHisto->GetNbinsX() + 1)) /
      2.;
    // --- Markus Elsing: fake high pt tracks, limit the integral
    if (IntPt < 0.0001)
      IntPt = 0.0001;
    P0 = P0 * IntPt;
  }
  double VxProb = 0.;
  const int sizeOfTPColl = vertex.trackParticleLinks().size();
  if (sizeOfTPColl > 0) {
    for (int j = 0; j < sizeOfTPColl; ++j) {
      /** @todo Given that you are looping from zero, the 'Power' and
       *'Factorial' could be speeded up by using a simple multiplication on each
       *loop (sroe)
       **/
      VxProb =
        VxProb + (TMath::Power(-TMath::Log(P0), j) / TMath::Factorial(j));
    }
    VxProb = -TMath::Log(VxProb * P0);
  }
  return VxProb;
}

} // Namespace

