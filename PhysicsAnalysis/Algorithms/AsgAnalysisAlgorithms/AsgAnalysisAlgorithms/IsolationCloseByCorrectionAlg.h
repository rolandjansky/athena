/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ASG_ANALYSIS_ALGORITHMS__ISOLATION_CLOSE_BY_CORRECTION_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__ISOLATION_CLOSE_BY_CORRECTION_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <IsolationSelection/IIsolationCloseByCorrectionTool.h>
#include <SelectionHelpers/OutOfValidityEventHelper.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IIsolationCloseByCorrectionTool

  class IsolationCloseByCorrectionAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    IsolationCloseByCorrectionAlg (const std::string& name, 
                                   ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// The OR toolbox
  private:
    ToolHandle<IIsolationCloseByCorrectionTool> m_isolationCorrectionTool {
      "CP::IsolationCloseByCorrectionTool", this};

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

  private:
    SysCopyHandle<xAOD::ElectronContainer> m_electronsHandle {
      this, "electrons", "", "the electrons container to use"};
    SysCopyHandle<xAOD::MuonContainer> m_muonsHandle {
      this, "muons", "", "the muons container to use"};
    SysCopyHandle<xAOD::PhotonContainer> m_photonsHandle {
      this, "photons", "", "the photons container to use"};

  private:
    int m_topoEtConeModel {IIsolationCloseByCorrectionTool::TopoConeCorrectionModel::DirectCaloClusters};

    /// \brief the helper for OutOfValidity results
  private:
    OutOfValidityEventHelper m_outOfValidity {this};
  };
}

#endif
