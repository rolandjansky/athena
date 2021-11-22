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
  }



  StatusCode BTaggingInformationDecoratorAlg ::
  initialize ()
  {
    if (m_taggerWeightDecoration.empty() && m_quantileDecoration.empty())
    {
      ANA_MSG_ERROR ("can't specify both tagger weight and quantile decorations are empty");
      return StatusCode::FAILURE;
    }

    ANA_CHECK (m_selectionTool.retrieve());
    ANA_CHECK (m_jetHandle.initialize (m_systematicsList));
    ANA_CHECK (m_taggerWeightDecoration.initialize (m_systematicsList, m_jetHandle, SG::AllowEmpty));
    ANA_CHECK (m_quantileDecoration.initialize (m_systematicsList, m_jetHandle, SG::AllowEmpty));
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
          if (m_taggerWeightDecoration)
          {
            double weight{-1.};
            ANA_CHECK_CORRECTION (m_outOfValidity, *jet, m_selectionTool->getTaggerWeight (*jet, weight));
            m_taggerWeightDecoration.set (*jet, weight, sys);
          }

          if (m_quantileDecoration)
          {
            const int quantile = m_selectionTool->getQuantile(*jet);
            m_quantileDecoration.set (*jet, quantile, sys);
          }
        } else {
          if (m_taggerWeightDecoration)
          {
            m_taggerWeightDecoration.set (*jet, -100., sys);
          }

          if (m_quantileDecoration)
          {
            m_quantileDecoration.set (*jet, -1, sys);
          }
        }
      }
    }
    return StatusCode::SUCCESS;
  }
}
