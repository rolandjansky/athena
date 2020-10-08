/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "CVFPrepAlg.h"
#include "StoreGate/DecorKeyHelpers.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/WriteDecorHandle.h"
#include "TrigEFMissingET/PUClassification.h"

namespace HLT
{
  namespace MET
  {
    CVFPrepAlg::CVFPrepAlg(const std::string &name, ISvcLocator *pSvcLocator)
        : AthReentrantAlgorithm(name, pSvcLocator)
    {
    }

    StatusCode CVFPrepAlg::initialize()
    {
      CHECK(m_inputClusterKey.initialize());

      if (m_inputCVFKey.key().find(".") == std::string::npos)
        m_inputCVFKey = m_inputClusterKey.key() + "." + m_inputCVFKey.key();
      else if (SG::contKeyFromKey(m_inputCVFKey.key()) != m_inputClusterKey.key())
      {
        ATH_MSG_ERROR("CVF key does not match input cluster container key!");
        return StatusCode::FAILURE;
      }
      CHECK(m_inputCVFKey.initialize());

      if (m_outputCategoryKey.key().find(".") == std::string::npos)
        m_outputCategoryKey = m_inputClusterKey.key() + "." + m_outputCategoryKey.key();
      else if (SG::contKeyFromKey(m_outputCategoryKey.key()) != m_inputClusterKey.key())
      {
        ATH_MSG_ERROR("Output category key does not match input cluster container key!");
        return StatusCode::FAILURE;
      }
      CHECK(m_outputCategoryKey.initialize());

      return StatusCode::SUCCESS;
    }

    StatusCode CVFPrepAlg::execute(const EventContext &ctx) const
    {
      auto clusters = SG::makeHandle(m_inputClusterKey, ctx);
      if (!clusters.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputClusterKey);
        return StatusCode::FAILURE;
      }
      auto accCVF = SG::makeHandle<float>(m_inputCVFKey, ctx);
      auto decCategory = SG::makeHandle<int>(m_outputCategoryKey, ctx);

      for (const xAOD::CaloCluster *iclus : *clusters)
      {
        float CVF = accCVF(*iclus);
        if (CVF < 0)
          decCategory(*iclus) = PUClassification::NeutralForward;
        else if (CVF < m_cvfThreshold)
          decCategory(*iclus) = PUClassification::ChargedPU;
        else
          decCategory(*iclus) = PUClassification::ChargedHS;
      }
      return StatusCode::SUCCESS;
    }

  } // namespace MET
} // namespace HLT