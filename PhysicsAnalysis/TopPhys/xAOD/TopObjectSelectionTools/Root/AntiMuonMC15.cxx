/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/AntiMuonMC15.h"

#include "TopEvent/EventTools.h"

namespace top {
  AntiMuonMC15::AntiMuonMC15(const double ptcut, AntiMuonIsolation* isolation) :
    m_ptcut(ptcut),
    m_muonSelectionTool("MuonSelectionTool"),
    m_isolation(isolation) {
    top::check(m_muonSelectionTool.retrieve(), "Failed to retrieve muonSelectionTool");
  }

  bool AntiMuonMC15::passSelection(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    ///-- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15 --///
    if (!m_muonSelectionTool->accept(mu)) return false;

    return m_isolation->passSelection(mu);
  }

  bool AntiMuonMC15::passSelectionLoose(const xAOD::Muon& /*mu*/) const {
    // code does not compile without this function

    return true;
  }

  void AntiMuonMC15::print(std::ostream& os) const {
    os << "AntiMuonMC15\n"
       << "    * pT > " << m_ptcut << "\n"
//         << "    * |eta| < " << m_etamax << "\n"
//         << "    * quality=" <<  m_quality << " (tight=0, medium=1, loose=2, v.loose=3)\n"
       << "    * Everything else from muon tool - fill this in?\n";

     if (!m_isolation)
       os << "    * No isolation requirement\n";
     else
       m_isolation->print(os);
  }
}
