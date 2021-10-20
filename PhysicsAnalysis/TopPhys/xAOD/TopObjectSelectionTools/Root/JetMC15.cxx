/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/JetMC15.h"

#include <cmath>

#include "TopEvent/EventTools.h"

namespace top {
  JetMC15::JetMC15(const double ptcut,
                   const double etamax,
                   const double minmass,
                   const bool doJVTCut):
    m_ptcut(ptcut),
    m_etamax(etamax),
    m_masscut(minmass),
    m_applyJVTCut(doJVTCut),
    m_appyMassCut(minmass > 0.),
    m_jvt_tool("JetJvtEfficiencyTool") {
    top::check(m_jvt_tool.retrieve(),
               "Failed to retrieve JVT tool");
  }

  // This version of the constructor always perform JVT cut 
  JetMC15::JetMC15(const double ptcut,
                   const double etamax) : JetMC15::JetMC15(ptcut, etamax, -1.0, true) {}

  // This version of the constructor always perform JVT cut 
  JetMC15::JetMC15(const double ptcut, const double etamax, const bool doJVTCut,
      const float jetElectronEMFractionMin, const float jetElectronEMFractionMax, const float jetElectronEtaMax) : JetMC15::JetMC15(ptcut, etamax, -1.0, doJVTCut) {
                m_useJetElectrons=true;
                m_jetElectronEMFractionMin=jetElectronEMFractionMin;
                // maximal jet-electron EM frac cut
                m_jetElectronEMFractionMax=jetElectronEMFractionMax;
                // maximal jet-electron eta cut
                m_jetElectronEtaMax=jetElectronEtaMax;
    }

  // DEPRECIATED - fwdJetSel string now defunct, keeping blank string input for backwards compatibility
  JetMC15::JetMC15(const double ptcut,
                   const double etamax,
                   const std::string) : JetMC15::JetMC15(ptcut, etamax, -1.0, true) {}

  // DEPRECATED - only kept for backward compatibility
  JetMC15::JetMC15(const double ptcut,
                   const double etamax,
                   const double) : JetMC15::JetMC15(ptcut, etamax) {}

  bool JetMC15::passSelection(const xAOD::Jet& jet) {
    // jet electrons
    bool JetElectron=false;
    if (m_useJetElectrons) {
      JetElectron = true;
      
      if (jet.pt() < m_ptcut)
        JetElectron = false;
      if (std::fabs(jet.eta()) > m_jetElectronEtaMax)
        JetElectron = false;
      
      if (std::fabs(jet.eta()) > 1.37 && std::fabs(jet.eta()) < 1.52)
        JetElectron = false;
      
      float emfrac = jet.auxdataConst<float>("EMFrac");
      if( emfrac > m_jetElectronEMFractionMax || emfrac < m_jetElectronEMFractionMin )
        JetElectron = false;
    }

    if (m_applyJVTCut) {
      jet.auxdecor<char>("passJVT") = (m_jvt_tool->passesJvtCut(jet) ? 1 : 0);
    }
    //fJVT pass/fail decision already attatched in CP tools

    if (jet.pt() < m_ptcut) return false;

    if (std::fabs(jet.eta()) > m_etamax) return false;

    // decorating with jet-electron flag
    if (m_useJetElectrons) {
      if(JetElectron)
        jet.auxdecor<int>("jet_electron")  = 1;
      else
        jet.auxdecor<int>("jet_electron")  = 0;
    }

    if (m_appyMassCut) {
      if (jet.m() < m_masscut) return false;
    }

    jet.auxdecor<char>("good") = 1;
    jet.auxdecor<char>("closeToLepton") = 0;

    return true;
  }

  void JetMC15::print(std::ostream& os) const {
    os << "JetMC15\n"
       << "    * pT > " << m_ptcut << "\n"
       << "    * |eta| < " << m_etamax << "\n";
    if (m_appyMassCut)
      os << "    * m > " << m_masscut << "\n";
  }
}  // namespace top
