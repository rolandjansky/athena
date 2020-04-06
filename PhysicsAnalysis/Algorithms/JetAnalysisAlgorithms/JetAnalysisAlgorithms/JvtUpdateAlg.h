/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef JET_ANALYSIS_ALGORITHMS__JVT_UPDATE_ALG_H
#define JET_ANALYSIS_ALGORITHMS__JVT_UPDATE_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <JetInterface/IJetUpdateJvt.h>
#include <SelectionHelpers/SelectionReadHandle.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>
#include <xAODJet/JetContainer.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IJetUpdateJvt

  class JvtUpdateAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    JvtUpdateAlg (const std::string& name, 
                  ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// \brief the update tool
  private:
    ToolHandle<IJetUpdateJvt> m_jvtTool;

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

    /// \brief the jet collection we run on
  private:
    SysCopyHandle<xAOD::JetContainer> m_jetHandle {
      this, "jets", "AntiKt4EMTopoJets", "the jet collection to run on"};

    /// \brief the preselection we apply to our input
  private:
    SelectionReadHandle m_preselection {
      this, "preselection", "", "the preselection to apply"};

    /// \brief the name of the decoration we create
  private:
    std::string m_decorationName {"Jvt"};

    /// \brief the decoration accessor we use
  private:
    std::unique_ptr<SG::AuxElement::Accessor<float> > m_decorationAccessor;
  };
}

#endif
