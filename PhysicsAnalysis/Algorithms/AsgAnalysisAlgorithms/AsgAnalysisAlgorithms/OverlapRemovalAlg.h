/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef ASG_ANALYSIS_ALGORITHMS__OVERLAP_REMOVAL_ALG_H
#define ASG_ANALYSIS_ALGORITHMS__OVERLAP_REMOVAL_ALG_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <AssociationUtils/ToolBox.h>
#include <SystematicsHandles/SysCopyHandle.h>
#include <SystematicsHandles/SysListHandle.h>

namespace CP
{
  /// \brief an algorithm for calling \ref IPileupReweightingTool

  class OverlapRemovalAlg final : public EL::AnaAlgorithm
  {
    /// \brief the standard constructor
  public:
    OverlapRemovalAlg (const std::string& name, 
                       ISvcLocator* pSvcLocator);


  public:
    StatusCode initialize () override;

  public:
    StatusCode execute () override;
    


    /// The OR toolbox
  private:
    ToolHandle<ORUtils::IOverlapRemovalTool> m_overlapTool {
      "ORUtils::OverlapRemovalTool", this};

    /// \brief the systematics list we run
  private:
    SysListHandle m_systematicsList {this};

  private:
    SysCopyHandle<xAOD::ElectronContainer> m_electronsHandle {
      this, "electrons", "", "the electrons container to use"};
    SysCopyHandle<xAOD::MuonContainer> m_muonsHandle {
      this, "muons", "", "the muons container to use"};
    SysCopyHandle<xAOD::JetContainer> m_jetsHandle {
      this, "jets", "", "the jets container to use"};
    SysCopyHandle<xAOD::TauJetContainer> m_tausHandle {
      this, "taus", "", "the taus container to use"};
    SysCopyHandle<xAOD::PhotonContainer> m_photonsHandle {
      this, "photons", "", "the photons container to use"};
    SysCopyHandle<xAOD::JetContainer> m_fatJetsHandle {
      this, "fatJets", "", "the fat jets container to use"};
  };
}

#endif
