/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak



//
// includes
//

#include <FTagAnalysisAlgorithms/BTaggingInformationDecoratorAlg.h>

//
// method implementations
//

namespace CP
{
  BTaggingInformationDecoratorAlg ::
  BTaggingInformationDecoratorAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
    , m_selectionTool ("", this)
  {
    declareProperty ("selectionTool", m_selectionTool, "the b-tagging selection tool");
    declareProperty ("taggerWeightDecoration", m_taggerWeightDecoration, "the decoration for the tagger weight");
    declareProperty ("quantileDecoration", m_quantileDecoration, "the decoration for the continuous WP quantile");
  }



  StatusCode BTaggingInformationDecoratorAlg ::
  initialize ()
  {
    if (m_taggerWeightDecoration.empty() && m_quantileDecoration.empty())
    {
      ANA_MSG_ERROR ("can't specify both tagger weight and quantile decorations are empty");
      return StatusCode::FAILURE;
    }

    if (!m_taggerWeightDecoration.empty())
    {
      m_taggerWeightDecorator = std::make_unique<SG::AuxElement::Decorator<float> > (m_taggerWeightDecoration);
    }
    if (!m_quantileDecoration.empty())
    {
      m_quantileDecorator = std::make_unique<SG::AuxElement::Decorator<int> > (m_quantileDecoration);
    }

    ANA_CHECK (m_selectionTool.retrieve());
    ANA_CHECK (m_jetHandle.initialize (m_systematicsList));
    ANA_CHECK (m_systematicsList.initialize());
    ANA_CHECK (m_preselection.initialize());
    ANA_CHECK (m_outOfValidity.initialize());

    return StatusCode::SUCCESS;
  }



  StatusCode BTaggingInformationDecoratorAlg ::
  execute ()
  {
    for (const auto& sys : m_systematicsList.systematicsVector())
    {
      const xAOD::JetContainer *jets{};
      ANA_CHECK (m_jetHandle.retrieve (jets, sys));
      for (const xAOD::Jet *jet : *jets)
      {
        if (m_preselection.getBool (*jet))
        {
          if (m_taggerWeightDecorator != nullptr)
          {
            double weight{-1.};
            ANA_CHECK_CORRECTION (m_outOfValidity, *jet, m_selectionTool->getTaggerWeight (*jet, weight));
            (*m_taggerWeightDecorator)(*jet) = weight;
          }

          if (m_quantileDecorator != nullptr)
          {
            const int quantile = m_selectionTool->getQuantile(*jet);
            (*m_quantileDecorator)(*jet) = quantile;
          }
        } else {
          if (m_taggerWeightDecorator != nullptr)
          {
            (*m_taggerWeightDecorator)(*jet) = -100.;
          }

          if (m_quantileDecorator != nullptr)
          {
            (*m_quantileDecorator)(*jet) = -1;
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
