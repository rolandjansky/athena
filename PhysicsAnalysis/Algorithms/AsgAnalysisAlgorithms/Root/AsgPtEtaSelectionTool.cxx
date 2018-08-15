/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgPtEtaSelectionTool.h>

#include <xAODEgamma/Electron.h>
#include <xAODBase/IParticle.h>
#include <cmath>

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
    declareProperty ("useClusterEta", m_useClusterEta, "whether to use the cluster eta (for electrons only)");
    declareProperty ("printCastWarning", m_printCastWarning, "whether to print a warning when the cast fails");
  }



  StatusCode AsgPtEtaSelectionTool ::
  initialize ()
  {
    if (m_minPt < 0 || !std::isfinite (m_minPt))
    {
      ATH_MSG_ERROR ("invalid value of minPt: " << m_minPt);
      return StatusCode::FAILURE;
    }
    if (m_maxEta < 0 || !std::isfinite (m_maxEta))
    {
      ATH_MSG_ERROR ("invalid value of maxEta: " << m_maxEta);
      return StatusCode::FAILURE;
    }
    if (m_etaGapLow < 0 || !std::isfinite (m_etaGapLow))
    {
      ATH_MSG_ERROR ("invalid value of etaGapLow: " << m_etaGapLow);
      return StatusCode::FAILURE;
    }
    if (m_etaGapHigh < 0 || !std::isfinite (m_etaGapHigh))
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
    if (m_useClusterEta)
      m_accept.addCut ("castElectron", "cast to electron");
    if (m_maxEta > 0)
      m_accept.addCut ("maxEta", "maximum eta cut");
    if (m_etaGapHigh > 0)
      m_accept.addCut ("etaGap", "eta gap cut");
    m_shouldPrintCastWarning = m_printCastWarning;

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

    if (m_minPt > 0)
      m_accept.setCutResult (cutIndex ++, particle->pt() >= m_minPt);

    if (m_maxEta > 0 || m_etaGapHigh > 0 || m_useClusterEta)
    {
      float absEta = 0;

      if (m_useClusterEta == true)
      {
        const xAOD::Electron *electron
          = dynamic_cast<const xAOD::Electron*>(particle);
        if (electron == nullptr)
        {
          if (m_shouldPrintCastWarning)
            ANA_MSG_ERROR ("failed to cast input particle to electron");
          m_shouldPrintCastWarning = false;
          return m_accept;
        }
        absEta = std::abs (electron->caloCluster()->etaBE(2));
      } else
      {
        absEta = std::abs (particle->eta());
      }

      if (m_maxEta > 0)
        m_accept.setCutResult (cutIndex ++, absEta <= m_maxEta);
      if (m_etaGapHigh > 0)
        m_accept.setCutResult (cutIndex ++, absEta < m_etaGapLow || absEta > m_etaGapHigh);
    }

    return m_accept;
  }
}
