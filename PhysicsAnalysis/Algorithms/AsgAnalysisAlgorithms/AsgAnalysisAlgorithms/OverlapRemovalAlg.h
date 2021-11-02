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
#include <SystematicsHandles/SysWriteDecorHandle.h>

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

    /// \brief the decoration for the overlap removal status
  private:
    SysWriteDecorHandle<char> m_electronsDecoration {
      this, "electronsDecoration", "", "the decoration for the electron overlap removal selection"};
    SysWriteDecorHandle<char> m_muonsDecoration {
      this, "muonsDecoration", "", "the decoration for the muon overlap removal selection"};
    SysWriteDecorHandle<char> m_jetsDecoration {
      this, "jetsDecoration", "", "the decoration for the jet overlap removal selection"};
    SysWriteDecorHandle<char> m_tausDecoration {
      this, "tausDecoration", "", "the decoration for the tau overlap removal selection"};
    SysWriteDecorHandle<char> m_photonsDecoration {
      this, "photonsDecoration", "", "the decoration for the photon overlap removal selection"};
    SysWriteDecorHandle<char> m_fatJetsDecoration {
      this, "fatJetsDecoration", "", "the decoration for the fat jet overlap removal selection"};

    /// \brief the tool output decoration for the overlap removal status
  private:
    std::string m_overlapRemovalDecoration;

    /// \brief the accessor for \ref m_overlapRemovalDecoration
  private:
    std::unique_ptr<const SG::AuxElement::Accessor<char> > m_overlapRemovalAccessor;

  };
}

#endif
