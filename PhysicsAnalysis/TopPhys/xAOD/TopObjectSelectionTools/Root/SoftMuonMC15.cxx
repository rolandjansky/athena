/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/SoftMuonMC15.h"

#include "TopEvent/EventTools.h"

namespace top {
  SoftMuonMC15::SoftMuonMC15(const double ptcut) :
    m_ptcut(ptcut),
    m_softmuonSelectionTool("SoftMuonSelectionTool") {
    top::check(m_softmuonSelectionTool.retrieve(), "Failed to retrieve softmuonSelectionTool");
  }

  bool SoftMuonMC15::passSelection(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    ///-- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15 --///
    if (!m_softmuonSelectionTool->accept(mu)) return false;

    return true;
  }

  void SoftMuonMC15::print(std::ostream& os) const {
    os << "SoftMuonMC15\n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * Everything else from muon tool - fill this in?\n";
  }
}
