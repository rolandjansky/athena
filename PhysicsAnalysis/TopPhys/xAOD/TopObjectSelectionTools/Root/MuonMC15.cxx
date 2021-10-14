/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/MuonMC15.h"

#include "TopEvent/EventTools.h"

#include "TopObjectSelectionTools/MsgCategory.h"
using namespace TopObjectSelectionTools;

namespace top {
  MuonMC15::MuonMC15(const double ptcut, IsolationBase* isolation, const double d0SigCut, const double delta_z0, const bool applyTTVACut) :
    m_ptcut(ptcut),
    m_d0SigCut(d0SigCut),
    m_delta_z0(delta_z0),
    m_muonSelectionTool("CP::MuonSelectionTool"),
    m_muonSelectionToolLoose("CP::MuonSelectionToolLoose"),
    m_isolation(isolation),
    m_applyTTVACut(applyTTVACut)
  {
    top::check(m_muonSelectionTool.retrieve(), "Failed to retrieve muonSelectionTool");
    top::check(m_muonSelectionToolLoose.retrieve(), "Failed to retrieve muonSelectionToolLoose");
  }

  MuonMC15::MuonMC15(const double ptcut, IsolationBase* isolation, const bool applyTTVACut) :
    MuonMC15::MuonMC15(ptcut, isolation, 3.0, 0.5, applyTTVACut) {}

  bool MuonMC15::passSelection(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    ///-- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15 --///
    if (!m_muonSelectionTool->accept(mu)) return false;

    //isolation, if m_isolation != nullptr
    if (m_isolation && !m_isolation->passSelection(mu)) return false;

    // Track-to-vertex association
    if (m_applyTTVACut) {
      if (!passTTVACuts(mu)) return false;
    }

    return true;
  }

  bool MuonMC15::passSelectionLoose(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    ///-- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15 --///
    if (!m_muonSelectionToolLoose->accept(mu)) return false;

    //isolation, if m_isolation != nullptr
    if (m_isolation && !m_isolation->passSelectionLoose(mu)) return false;

    // Track-to-vertex association
    if (m_applyTTVACut) {
      if (!passTTVACuts(mu)) return false;
    }

    return true;
  }

  bool MuonMC15::passTTVACuts(const xAOD::Muon& mu) const {
    // TTVA:
    // see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPEOYE2015#Track_to_Vertex_Association
    if (!mu.isAvailable<float>("d0sig")) {
      ATH_MSG_WARNING("d0 significance not found for muon. "
          << "Maybe no primary vertex? Won't accept.");
      return false;
    }

    float d0sig = mu.auxdataConst<float>("d0sig");
    if (std::abs(d0sig) >= m_d0SigCut) return false;

    if (!mu.isAvailable<float>("delta_z0_sintheta")) {
      ATH_MSG_WARNING("delta z0*sin(theta) not found for muon. "
          << "Maybe no primary vertex? Won't accept.");
      return false;
    }

    float delta_z0_sintheta = mu.auxdataConst<float>("delta_z0_sintheta");
    if (std::abs(delta_z0_sintheta) >= m_delta_z0) return false;

    return true;
  }

  void MuonMC15::print(std::ostream& os) const {
    os << "MuonMC15\n"
       << "    * pT > " << m_ptcut << "\n"
//         << "    * |eta| < " << m_etamax << "\n"
//         << "    * quality=" <<  m_quality << " (tight=0, medium=1, loose=2, v.loose=3)\n"
       << "    * Everything else from muon tool - fill this in?\n";

    if (!m_isolation) os << "    * No isolation requirement\n";
    else m_isolation->print(os);
  }
}
