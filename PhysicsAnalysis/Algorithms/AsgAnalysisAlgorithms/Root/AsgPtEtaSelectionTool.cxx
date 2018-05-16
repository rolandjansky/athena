/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgPtEtaSelectionTool.h>

#include <xAODBase/IParticle.h>

//
// method implementations
//

namespace CP
{
  AsgPtEtaSelectionTool :: 
  AsgPtEtaSelectionTool (const std::string& name)
    : AsgTool (name)
  {
    declareProperty ("minPt", m_minPt, "minimum pt to require (or 0 for no pt cut)");
    declareProperty ("maxEta", m_maxEta, "maximum abs(eta) to allow (or 0 for no eta cut)");
    declareProperty ("etaGapLow", m_etaGapLow, "low end of the eta gap");
    declareProperty ("etaGapHigh", m_etaGapHigh, "high end of the eta gap (or 0 for no eta gap)");
  }



  StatusCode AsgPtEtaSelectionTool ::
  initialize ()
  {
    if (m_minPt < 0 || !isfinite (m_minPt))
    {
      ATH_MSG_ERROR ("invalid value of minPt: " << m_minPt);
      return StatusCode::FAILURE;
    }
    if (m_maxEta < 0 || !isfinite (m_maxEta))
    {
      ATH_MSG_ERROR ("invalid value of maxEta: " << m_maxEta);
      return StatusCode::FAILURE;
    }
    if (m_etaGapLow < 0 || !isfinite (m_etaGapLow))
    {
      ATH_MSG_ERROR ("invalid value of etaGapLow: " << m_etaGapLow);
      return StatusCode::FAILURE;
    }
    if (m_etaGapHigh < 0 || !isfinite (m_etaGapHigh))
    {
      ATH_MSG_ERROR ("invalid value of etaGapHigh: " << m_etaGapHigh);
      return StatusCode::FAILURE;
    }
    if (m_etaGapHigh > 0 && m_etaGapLow >= m_etaGapHigh)
    {
      ATH_MSG_ERROR ("invalid eta gap: " << m_etaGapLow << " to " << m_etaGapHigh);
      return StatusCode::FAILURE;
    }
    if (m_etaGapHigh > 0 && m_maxEta > 0 && m_etaGapHigh >= m_maxEta)
    {
      ATH_MSG_ERROR ("etaGapHigh=" << m_etaGapHigh << " >= maxEta=" << m_maxEta);
      return StatusCode::FAILURE;
    }

    if (m_minPt > 0)
      m_accept.addCut ("minPt", "minimum pt cut");
    if (m_maxEta > 0)
      m_accept.addCut ("maxEta", "maximum eta cut");
    if (m_etaGapHigh > 0)
      m_accept.addCut ("etaGap", "eta gap cut");

    return StatusCode::SUCCESS;
  }



  const Root::TAccept& AsgPtEtaSelectionTool ::
  getTAccept () const
  {
    return m_accept;
  }



  const Root::TAccept& AsgPtEtaSelectionTool ::
  accept (const xAOD::IParticle *particle) const
  {
    m_accept.clear();
    std::size_t cutIndex {};
    const float absEta = std::abs (particle->eta());

    if (m_minPt > 0)
      m_accept.setCutResult (cutIndex ++, particle->pt() >= m_minPt);
    if (m_maxEta > 0)
      m_accept.setCutResult (cutIndex ++, absEta <= m_maxEta);
    if (m_etaGapHigh > 0)
      m_accept.setCutResult (cutIndex ++, absEta < m_etaGapLow || absEta > m_etaGapHigh);

    return m_accept;
  }
}
