/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Tadej Novak



#ifndef F_TAG_ANALYSIS_ALGORITHMS__B_TAGGING_INFORMATION_DECORATION_ALG_H
#define F_TAG_ANALYSIS_ALGORITHMS__B_TAGGING_INFORMATION_DECORATION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <FTagAnalysisInterfaces/IBTaggingSelectionTool.h>
#include <SelectionHelpers/OutOfValidityHelper.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysReadHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <SystematicsHandles/SysWriteDecorHandle.h>
#include <xAODJet/JetContainer.h>

namespace CP
{
  class BTaggingInformationDecoratorAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    BTaggingInformationDecoratorAlg (const std::string& name, 
                                     ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the selection tool
  private:
    ToolHandle<IBTaggingSelectionTool> m_selectionTool;

    /// \brief the jets continer we run on
  private:
    SysReadHandle<xAOD::JetContainer> m_jetHandle {
      this, "jets", "", "the jets collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityHelper m_outOfValidity {this};

    /// \brief the decoration for the b-tagging weight
  private:
    std::string m_taggerWeightDecoration {};

    /// \brief the decorator for \ref m_taggerWeightDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Decorator<float> > m_taggerWeightDecorator {};

    /// \brief the decoration for the b-tagging quantiles
  private:
    std::string m_quantileDecoration {};

    /// \brief the decorator for \ref m_quantileDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Decorator<int> > m_quantileDecorator {};
  };
}

#endif
