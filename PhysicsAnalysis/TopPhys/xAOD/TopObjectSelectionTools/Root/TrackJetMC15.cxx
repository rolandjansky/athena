/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/TrackJetMC15.h"

namespace top{

  TrackJetMC15::TrackJetMC15(const double ptcut,const double etamax) :
    m_ptcut(ptcut),
    m_etamax(etamax) {
    }

  TrackJetMC15::~TrackJetMC15() {
  }

  bool TrackJetMC15::passSelection(const xAOD::Jet& jet) {
    
    if (jet.pt() < m_ptcut)
      return false;

    if (std::fabs(jet.eta()) > m_etamax)
      return false;

    // See https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TrackJetSelectionRecAndGuide
    if (jet.numConstituents() < 2)
      return false;

    return true;
  }

  void TrackJetMC15::print(std::ostream& os) const {
    os << "Track jet\n"
      << "    * pT > " << m_ptcut << "\n"
      << "    * |eta| < " << m_etamax << "\n"
      << "    * >= 2 tracks \n";
  }
}
