/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "FlowElementPrepAlg.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/DecorKeyHelpers.h"
#include "TrigEFMissingET/PUClassification.h"
#include "AthContainers/ConstDataVector.h"

/// Anonymous namespace
namespace
{
  const static SG::AuxElement::ConstAccessor<char> accPVMatched("matchedToPV");
}

namespace HLT
{
  namespace MET
  {
    FlowElementPrepAlg::FlowElementPrepAlg(const std::string &name, ISvcLocator *pSvcLocator)
        : AthReentrantAlgorithm(name, pSvcLocator)
    {
    }

    StatusCode FlowElementPrepAlg::initialize()
    {
      m_manualTVA = !m_tvaTool.empty();
      CHECK(m_inputNeutralKey.initialize());
      CHECK(m_inputChargedKey.initialize());
      CHECK(m_outputKey.initialize());
      if (m_outputCategoryKey.key().find(".") == std::string::npos)
        m_outputCategoryKey = m_outputKey.key() + "." + m_outputCategoryKey.key();
      else if (SG::contKeyFromKey(m_outputCategoryKey.key()) != m_outputKey.key())
      {
        ATH_MSG_ERROR("Category key does not match output container key!");
        return StatusCode::FAILURE;
      }
      CHECK(m_outputCategoryKey.initialize());
      m_decCategory.emplace(SG::decorKeyFromKey(m_outputCategoryKey.key()));

      if (m_manualTVA)
      {
        CHECK(m_tvaTool.retrieve());
        if (!m_trackSelTool.empty())
          CHECK(m_trackSelTool.retrieve());
      }
      CHECK(m_inputVertexKey.initialize(m_manualTVA));
      return StatusCode::SUCCESS;
    }

    StatusCode FlowElementPrepAlg::execute(const EventContext &ctx) const
    {
      auto charged = SG::makeHandle(m_inputChargedKey, ctx);
      if (!charged.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputChargedKey);
        return StatusCode::FAILURE;
      }
      auto neutral = SG::makeHandle(m_inputNeutralKey, ctx);
      if (!neutral.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_inputNeutralKey);
        return StatusCode::FAILURE;
      }
      const xAOD::Vertex *priVtx = nullptr;
      std::vector<const xAOD::Vertex *> vertices;
      if (m_manualTVA)
      {
        auto vertexHandle = SG::makeHandle(m_inputVertexKey, ctx);
        if (!vertexHandle.isValid())
        {
          ATH_MSG_ERROR("Failed to retrieve " << m_inputVertexKey);
          return StatusCode::FAILURE;
        }
        // Find the primary vertex and build up the vector for the TVA tool
        vertices.reserve(vertexHandle->size());
        for (const xAOD::Vertex *ivtx : *vertexHandle)
        {
          if (!priVtx && ivtx->vertexType() == xAOD::VxType::PriVtx)
            priVtx = ivtx;
          vertices.push_back(ivtx);
        }
      }
      auto outputHandle = SG::makeHandle(m_outputKey, ctx);
      auto &decCategory = *m_decCategory;

      auto combined = std::make_unique<ConstDataVector<xAOD::FlowElementContainer>>(SG::VIEW_ELEMENTS);
      combined->reserve(charged->size() + neutral->size());

      for (const xAOD::FlowElement *ife : *charged)
      {
        combined->push_back(ife);
        if (m_manualTVA)
        {
          if (!priVtx)
            // If we could not identify a primary vertex, classify as neutral
            // Note that this means in order to distinguish cPFOs from nPFOs the 'isCharged'
            // function should still be used
            decCategory(*ife) = PUClassification::NeutralForward;
          else
          {
            const xAOD::IParticle *icharged = ife->chargedObject(0);
            const xAOD::TrackParticle *itrk = dynamic_cast<const xAOD::TrackParticle *>(icharged);
            if (!itrk)
            {
              ATH_MSG_ERROR("Could not retrieve primary track particle from FlowElement");
              return StatusCode::FAILURE;
            }
            if (itrk && (m_trackSelTool.empty() || m_trackSelTool->accept(itrk)))
            {
              bool isPVMatched;
              if (m_useCompatible)
                isPVMatched = m_tvaTool->isCompatible(*itrk, *priVtx);
              else
                isPVMatched = m_tvaTool->getUniqueMatchVertex(*itrk, vertices) == priVtx;
              if (isPVMatched)
                decCategory(*ife) = PUClassification::ChargedHS;
              else
                decCategory(*ife) = PUClassification::ChargedPU;
            }
          }
        }
        else
        {
          if (accPVMatched(*ife))
            decCategory(*ife) = PUClassification::ChargedHS;
          else
            decCategory(*ife) = PUClassification::ChargedPU;
        }
      }
      for (const xAOD::FlowElement *ife : *neutral)
      {
        combined->push_back(ife);
        decCategory(*ife) = PUClassification::NeutralForward;
      }

      CHECK(outputHandle.put(ctx, std::move(combined)) != nullptr);
      return StatusCode::SUCCESS;
    }
  } // namespace MET
} // namespace HLT
