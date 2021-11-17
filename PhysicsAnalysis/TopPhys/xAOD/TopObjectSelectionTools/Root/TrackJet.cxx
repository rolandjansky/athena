/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/TrackJet.h"

namespace top {
  TrackJet::TrackJet(const double ptcut, const double etamax) :
    m_ptcut(ptcut),
    m_etamax(etamax) {
  }

  TrackJet::~TrackJet() {
  }

  bool TrackJet::passSelection(const xAOD::Jet& jet) {
    if (jet.pt() < m_ptcut) return false;

    if (std::fabs(jet.eta()) > m_etamax) return false;

    // See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackJetSelectionRecAndGuide
    if (jet.numConstituents() < 2) return false;

    return true;
  }

  void TrackJet::print(std::ostream& os) const {
    os << "TrackJet\n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * |eta| < " << m_etamax << "\n"
       << "    * >= 2 tracks \n";
  }
}
