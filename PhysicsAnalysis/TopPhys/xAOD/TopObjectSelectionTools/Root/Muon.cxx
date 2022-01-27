/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/Muon.h"

#include "TopEvent/EventTools.h"

#include "TopObjectSelectionTools/MsgCategory.h"
using namespace TopObjectSelectionTools;

namespace top {
  Muon::Muon(const double ptcut, IsolationBase* isolation,
             const double d0SigCut, const double delta_z0_sintheta,
             const bool applyTTVACut) :
    m_ptcut(ptcut),
    m_d0SigCut(d0SigCut),
    m_delta_z0(delta_z0_sintheta),
    m_muonSelectionTool("MuonSelectionTool"),
    m_muonSelectionToolLoose("MuonSelectionToolLoose"),
    m_isolation(isolation),
    m_applyTTVACut(applyTTVACut) {
    top::check(m_muonSelectionTool.retrieve(), "Failed to retrieve muonSelectionTool");
    top::check(m_muonSelectionToolLoose.retrieve(), "Failed to retrieve muonSelectionToolLoose");
  }

  Muon::Muon(const double ptcut, IsolationBase* isolation, const bool applyTTVACut)
    : Muon::Muon(ptcut, isolation, 3.0, 0.5, applyTTVACut) {}

  Muon::Muon(const double ptcut, IsolationBase* isolation)
    : Muon::Muon(ptcut, isolation, 3.0, 0.5, true) {}

  bool Muon::passSelection(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    if (!m_muonSelectionTool->accept(mu)) return false;

    //isolation, if m_isolation != nullptr
    if (m_isolation && !m_isolation->passSelection(mu)) return false;

    // Track-to-vertex association
    if (m_applyTTVACut) {
      if (!passTTVACuts(mu)) return false;
    }

    return true;
  }

  bool Muon::passSelectionLoose(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    if (!m_muonSelectionToolLoose->accept(mu)) return false;

    //isolation, if m_isolation != nullptr
    if (m_isolation && !m_isolation->passSelectionLoose(mu)) return false;

    // Track-to-vertex association
    if (m_applyTTVACut) {
      if (!passTTVACuts(mu)) return false;
    }

    return true;
  }

  bool Muon::passTTVACuts(const xAOD::Muon& mu) const {
    // TTVA:
    // see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackingCPEOYE2015#Track_to_Vertex_Association
    if (!mu.isAvailable<float>("d0sig")) {
      ATH_MSG_WARNING("d0 significance not found for muon. "
          << "Maybe no primary vertex? Won't accept.");
      return false;
    }

    float d0sig = mu.auxdataConst<float>("d0sig");
    if (std::abs(d0sig) >= 3) return false;

    if (!mu.isAvailable<float>("delta_z0_sintheta")) {
      ATH_MSG_WARNING("delta z0*sin(theta) not found for muon. "
          << "Maybe no primary vertex? Won't accept.");
      return false;
    }

    float delta_z0_sintheta = mu.auxdataConst<float>("delta_z0_sintheta");
    if (std::abs(delta_z0_sintheta) >= 0.5) return false;

    return true;
  }

  void Muon::print(std::ostream& os) const {
    os << "Muon\n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * Everything else from muon tool - fill this in?\n";

    if (!m_isolation) os << "    * No isolation requirement\n";
    else m_isolation->print(os);
  }
}
