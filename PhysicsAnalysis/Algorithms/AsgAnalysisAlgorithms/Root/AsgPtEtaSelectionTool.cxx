/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgPtEtaSelectionTool.h>

#include <xAODEgamma/Egamma.h>
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
    declareProperty ("maxPt", m_maxPt, "maximum pt to require (or 0 for no pt cut)");
    declareProperty ("maxEta", m_maxEta, "maximum abs(eta) to allow (or 0 for no eta cut)");
    declareProperty ("etaGapLow", m_etaGapLow, "low end of the eta gap");
    declareProperty ("etaGapHigh", m_etaGapHigh, "high end of the eta gap (or 0 for no eta gap)");
    declareProperty ("useClusterEta", m_useClusterEta, "whether to use the cluster eta (for electrons only)");
    declareProperty ("printCastWarning", m_printCastWarning, "whether to print a warning/error when the cast fails");
    declareProperty ("printClusterWarning", m_printClusterWarning, "whether to print a warning/error when the cluster is missing");
  }



  StatusCode AsgPtEtaSelectionTool ::
  initialize ()
  {
    if (m_minPt < 0 || !std::isfinite (m_minPt))
    {
      ATH_MSG_ERROR ("invalid value of minPt: " << m_minPt);
      return StatusCode::FAILURE;
    }
    if (m_maxPt < 0 || !std::isfinite (m_maxPt))
    {
      ATH_MSG_ERROR ("invalid value of m_maxPt: " << m_maxPt);
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

    if (m_minPt > 0) {
       ATH_MSG_DEBUG( "Performing pt >= " << m_minPt << " MeV selection" );
       m_minPtCutIndex = m_accept.addCut ("minPt", "minimum pt cut");
    }
    if (m_maxPt > 0) {
       ATH_MSG_DEBUG( "Performing pt < " << m_maxPt << " MeV selection" );
       m_maxPtCutIndex = m_accept.addCut ("maxPt", "maximum pt cut");
    }
    if (m_useClusterEta) {
       ATH_MSG_DEBUG( "Performing eta cut on the e/gamma cluster" );
       m_egammaCastCutIndex = m_accept.addCut ("castEgamma", "cast to egamma");
       m_egammaClusterCutIndex = m_accept.addCut ("caloCluster", "egamma object has cluster");
    }
    if (m_maxEta > 0) {
       ATH_MSG_DEBUG( "Performing |eta| < " << m_maxEta << " selection" );
       m_maxEtaCutIndex = m_accept.addCut ("maxEta", "maximum eta cut");
    }
    if (m_etaGapHigh > 0) {
       ATH_MSG_DEBUG( "Performing !( " << m_etaGapLow << " < |eta| < "
                      << m_etaGapHigh << " ) selection" );
       m_etaGapCutIndex = m_accept.addCut ("etaGap", "eta gap cut");
    }
    m_shouldPrintCastWarning = m_printCastWarning;
    m_shouldPrintClusterWarning = m_printClusterWarning;

    return StatusCode::SUCCESS;
  }



  const asg::AcceptInfo& AsgPtEtaSelectionTool ::
  getAcceptInfo () const
  {
    return m_accept;
  }



  asg::AcceptData AsgPtEtaSelectionTool ::
  accept (const xAOD::IParticle *particle) const
  {
    asg::AcceptData accept (&m_accept);

    // Perform the tranverse momentum cuts.
    if (m_minPtCutIndex >= 0) {
       accept.setCutResult (m_minPtCutIndex, particle->pt() >= m_minPt);
    }
    if (m_maxPtCutIndex >= 0) {
       accept.setCutResult (m_maxPtCutIndex, particle->pt() < m_maxPt);
    }

    // Perform the eta cut(s).
    if (m_maxEtaCutIndex >= 0 || m_etaGapCutIndex >= 0)
    {
      float absEta = 0;

      if (m_useClusterEta == true)
      {
        const xAOD::Egamma *egamma
          = dynamic_cast<const xAOD::Egamma*>(particle);
        if (egamma == nullptr)
        {
          if (m_shouldPrintCastWarning)
            ANA_MSG_ERROR ("failed to cast input particle to electron");
          m_shouldPrintCastWarning = false;
          return accept;
        }
        accept.setCutResult (m_egammaCastCutIndex, true);
        const xAOD::CaloCluster *const caloCluster {egamma->caloCluster()};
        if (!caloCluster)
        {
          if (m_shouldPrintClusterWarning)
            ANA_MSG_ERROR ("no calo-cluster associated with e-gamma object");
          m_shouldPrintClusterWarning = false;
          return accept;
        }
        accept.setCutResult (m_egammaClusterCutIndex, true);
        absEta = std::abs (caloCluster->etaBE(2));
      } else
      {
        absEta = std::abs (particle->eta());
      }

      if (m_maxEtaCutIndex >= 0) {
        accept.setCutResult (m_maxEtaCutIndex, absEta <= m_maxEta);
      }
      if (m_etaGapCutIndex >= 0) {
        accept.setCutResult (m_etaGapCutIndex, (absEta < m_etaGapLow ||
                                                  absEta > m_etaGapHigh));
      }
    }

    return accept;
  }
}
