/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/AntiMuon.h"

#include "TopEvent/EventTools.h"

namespace top {
  AntiMuon::AntiMuon(const double ptcut, AntiMuonIsolation* isolation) :
    m_ptcut(ptcut),
    m_muonSelectionTool("MuonSelectionTool"),
    m_isolation(isolation) {
    top::check(m_muonSelectionTool.retrieve(), "Failed to retrieve muonSelectionTool");
  }

  bool AntiMuon::passSelection(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    if (!m_muonSelectionTool->accept(mu)) return false;

    return m_isolation->passSelection(mu);
  }

  bool AntiMuon::passSelectionLoose(const xAOD::Muon& /*mu*/) const {
    // code does not compile without this function

    return true;
  }

  void AntiMuon::print(std::ostream& os) const {
    os << "AntiMuon\n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * Everything else from muon tool - fill this in?\n";

     if (!m_isolation)
       os << "    * No isolation requirement\n";
     else
       m_isolation->print(os);
  }
}
