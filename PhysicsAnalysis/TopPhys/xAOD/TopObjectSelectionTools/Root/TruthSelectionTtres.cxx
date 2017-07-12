/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/TruthSelectionTtres.h"
#include "TopEvent/EventTools.h"

namespace top {

  TruthSelectionTtres::TruthSelectionTtres()
  {
  }

  bool TruthSelectionTtres::passSelection(const xAOD::TruthParticle& mc) const
  {
    int p = (int) std::fabs(mc.pdgId());
    int s = mc.status();
    int b = mc.barcode();

    if (b > 100000) return false;
    if (p == 11 || p == 13) {
      if (s != 1) // only final electrons and muons for e-in-jet and isolation performance eval.
        return false;
      return true;
    }

    if (p == 6) {
      if (s != 3) // tops after radiation for mtt reconstruction
        return false;
      return true;
    }

    if (isBhadron(p) || isChadron(p)) {
      if (s != 3) // b and c hadrons with status 3 to match with jets
        return false;
      return true;
    }

    if (p == 12 || p == 14 || p == 16) {
      if (s != 1) // only final neutrinos for m_tt reconstruction
        return false;
      return true;
    }

    // veto everything else
    return false;
  }

  void TruthSelectionTtres::print(std::ostream& os) const {
      os << "TruthSelectionTtres\n"
        << "    * keep final electrons and muons\n"
        << "    * keep top quarks after radiation\n"
        << "    * keep b and c hadrons\n"
        << "    * keep final neutrinos\n"
        ;
  }

}
