/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/SoftMuon.h"

#include "TopEvent/EventTools.h"

namespace top {
  SoftMuon::SoftMuon(const double ptcut) :
    m_ptcut(ptcut),
    m_softmuonSelectionTool("SoftMuonSelectionTool") {
    top::check(m_softmuonSelectionTool.retrieve(), "Failed to retrieve softmuonSelectionTool");
  }

  bool SoftMuon::passSelection(const xAOD::Muon& mu) const {
    if (mu.pt() < m_ptcut) return false;

    if (!m_softmuonSelectionTool->accept(mu)) return false;

    return true;
  }

  void SoftMuon::print(std::ostream& os) const {
    os << "SoftMuon\n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * Everything else from muon tool - fill this in?\n";
  }
}
