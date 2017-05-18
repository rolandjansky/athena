/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/JetMC15.h"

#include <cmath>

#include "TopEvent/EventTools.h"

namespace top {

  JetMC15::JetMC15(const double ptcut,
                   const double etamax,
                   const bool doJVTCut,
                   const std::string fwdJetSel) :
    m_ptcut(ptcut),
    m_etamax(etamax),
    m_applyJVTCut(doJVTCut),
    m_fwdJetSel(fwdJetSel),
    m_jvt_tool("JetJvtEfficiencyTool") {
    top::check(m_jvt_tool.retrieve(),
               "Failed to retrieve JVT tool");
  }

  // This version of the constructor always perform JVT cut
  JetMC15::JetMC15(const double ptcut,
                   const double etamax,
                   const std::string fwdJetSel) : JetMC15::JetMC15(ptcut, etamax, true, fwdJetSel) {}

  // DEPRECATED - only kept for backward compatibility
  JetMC15::JetMC15(const double ptcut,
                   const double etamax,
                   const double) : JetMC15::JetMC15(ptcut, etamax) {}

  bool JetMC15::passSelection(const xAOD::Jet& jet) {
    if (jet.pt() < m_ptcut)
        return false;

    if (std::fabs(jet.eta()) > m_etamax)
        return false;

    if (m_applyJVTCut) {
      if (!m_jvt_tool->passesJvtCut(jet)) {
        jet.auxdecor<char>("passJVT")          = 0;
      }
      else {
        jet.auxdecor<char>("passJVT")          = 1;
      }
    }

    if (m_fwdJetSel == "fJVT") {
      if (!jet.getAttribute<char>("passFJVT"))
        return false;
    }
    else if (m_fwdJetSel == "Tight") {
      if (std::fabs(jet.eta()) > 2.5 && jet.pt() < 30e3)
        return false;
    }

    jet.auxdecor<char>("good")          = 1;
    jet.auxdecor<char>("closeToLepton") = 0;

    return true;
  }

  void JetMC15::print(std::ostream& os) const {
    os << "JetMC15\n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * |eta| < " << m_etamax << "\n";
  }  
}  // namespace top
