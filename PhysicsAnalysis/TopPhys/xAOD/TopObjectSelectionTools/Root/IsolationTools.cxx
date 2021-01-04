/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopObjectSelectionTools/IsolationTools.h"

#include "TopConfiguration/ConfigurationSettings.h"
#include "TopEvent/EventTools.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"

#include "TopObjectSelectionTools/MsgCategory.h"
using namespace TopObjectSelectionTools;

namespace top {
  IsolationBase::IsolationBase() {
  }

  IsolationBase::~IsolationBase() {
  }

  AbsoluteIsolationDC14::AbsoluteIsolationDC14(const xAOD::Iso::IsolationType type, double cutvalue) :
    m_type(type),
    m_cutvalue(cutvalue) {
  }

  bool AbsoluteIsolationDC14::passSelection(const xAOD::IParticle& p) const {
    //electrons
    if (p.type() == xAOD::Type::Electron) {
      float iso = 0.;
      const xAOD::Electron* el = dynamic_cast<const xAOD::Electron*>(&p);
      bool exists = el->isolationValue(iso, m_type);
      return exists && iso > m_cutvalue;
    }

    //muons
    if (p.type() == xAOD::Type::Muon) {
      float iso = 0.;
      const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&p);
      bool exists = mu->isolation(iso, m_type);
      return exists && iso > m_cutvalue;
    }

    throw std::runtime_error("TopObjectSelectionTools::IsolationTools::AbsoluteIsolationDC14: Not an electron or muon");
  }

///Implement me!
  bool AbsoluteIsolationDC14::passSelectionLoose(const xAOD::IParticle& /*p*/) const {
    return true;
  }

  void AbsoluteIsolationDC14::print(std::ostream& os) const {
    os << "    * AbsoluteIsolationDC14\n";
    os << "        * " << m_type << " > " << m_cutvalue << " MeV\n";
    //missing in xaodprimities-00-00-02 os << "        * " << xAOD::Iso::toString(m_type) << " > " << m_cutvalue << "
    // MeV\n";
  }

  RelativePTVarCone::RelativePTVarCone(unsigned int size, double fraction, unsigned int sizeLoose,
                                       double fractionLoose) :
    m_fraction(fraction),
    m_fractionLoose(fractionLoose) {
    if (size != 20 && size != 30 && size != 40) {
      throw std::runtime_error("TopObjectSelectionTools::IsolationTools::RelativePTVarCone: "
                               "ptvarcone only supports cone sizes of 20, 30, 40\n and you asked for " + std::to_string (size));
    }

    std::stringstream ss;
    ss << "ptvarcone" << size;
    m_size = ss.str();

    ss.str("");
    ss << "ptvarcone" << sizeLoose;
    m_sizeLoose = ss.str();
  }

  bool RelativePTVarCone::passSelection(const xAOD::IParticle& p) const {
    return(p.auxdataConst<float>(m_size) / p.pt() < m_fraction);
  }

  bool RelativePTVarCone::passSelectionLoose(const xAOD::IParticle& p) const {
    return(p.auxdataConst<float>(m_sizeLoose) / p.pt() < m_fractionLoose);
  }

  void RelativePTVarCone::print(std::ostream& os) const {
    os << "    * RelativePTVarCone\n";
    os << "        * " << m_size << " /pT > " << m_fraction << "\n";
  }

  ApproxPTVarCone::ApproxPTVarCone(double fraction, double fractionLoose) :
    m_fraction(fraction),
    m_fractionLoose(fractionLoose) {
  }

  bool ApproxPTVarCone::passSelection(const xAOD::IParticle& p) const {
    return(p.auxdataConst<float>("miniIso") / p.pt() < m_fraction);
  }

  bool ApproxPTVarCone::passSelectionLoose(const xAOD::IParticle& p) const {
    return(p.auxdataConst<float>("miniIso") / p.pt() < m_fractionLoose);
  }

  void ApproxPTVarCone::print(std::ostream& os) const {
    os << "    * Approximate Mini Isolation\n";
    os << "        * iso/pT > " << m_fraction << "\n";
  }
  
  AntiMuonIsolation::AntiMuonIsolation(const std::string& workingPoint) :
        m_workingPoint(workingPoint) {
    if (m_workingPoint.substr(0,9) != "AntiMuon_") {
      ATH_MSG_WARNING("Ignoring isolation working point \""
          << workingPoint
          << "\" which is not appropriate for AntiMuons.\n"
          << "Will use \"AntiMuon_nominal\" instead.");
      m_workingPoint = "AntiMuon_nominal";
    }
    else if (m_workingPoint != "AntiMuon_nominal"
      && m_workingPoint != "AntiMuon_shapeSyst1"
      && m_workingPoint != "AntiMuon_shapeSyst2") {
      ATH_MSG_ERROR("Cannot use undefined isolation working point "
          << workingPoint
          << " for AntiMuons.\n"
          << "Should be one of \"AntiMuon_nominal\", \"AntiMuon_shapeSyst1\", \"AntiMuon_shapeSyst2\".");
      throw std::runtime_error("Attempt to use inappropriate isolation working point for AntiMuons");
    }
  }
  
  bool AntiMuonIsolation::passSelection(const xAOD::IParticle& p) const {
    //muons
    if (p.type() != xAOD::Type::Muon) {
      ATH_MSG_ERROR("Cannot use this function for anything else but muons");
      throw std::runtime_error("Cannot use this function for anything else but muons");
    }
    
    const xAOD::Muon* mu = dynamic_cast<const xAOD::Muon*>(&p);
    if (mu == nullptr) {
      ATH_MSG_ERROR("Impossible to cast pointer to xAOD::IParticle into pointer to xAOD::Muon");
      throw std::runtime_error("Impossible to cast pointer to xAOD::IParticle into pointer to xAOD::Muon");
    }
    
    if (mu->energyLossType() != xAOD::Muon::NotIsolated) return false;

    float eloss = 0;
    bool ok = mu->parameter(eloss, xAOD::Muon::EnergyLoss);
    if (ok && eloss > 6000) return false;

    float etcone20 = 0, ptvarcone40 = 0;
    ok = mu->isolation(etcone20, xAOD::Iso::etcone20);
    if (ok && etcone20 / mu->pt() < 0.03) return false;

    ok = mu->isolation(ptvarcone40, xAOD::Iso::ptvarcone40);
    if (m_workingPoint == "AntiMuon_nominal") {
      if (ok && ptvarcone40 / mu->pt() > 0.1) return false;
    }
    else if (m_workingPoint == "AntiMuon_shapeSyst1") {
      if (ok && ptvarcone40 / mu->pt() > 0.05) return false;
    }
    else if (m_workingPoint == "AntiMuon_shapeSyst2") {
      if (ok && (ptvarcone40 / mu->pt() <= 0.05 || ptvarcone40 / mu->pt() > 0.1)) return false;
    }

    return true;
  }

  void AntiMuonIsolation::print(std::ostream& os) const {
    os << "    * AntiMuon Isolation : " << m_workingPoint << "\n";
    os << "        * energyLossType : NotIsolated\n";
    os << "        * EnergyLoss <= 6 GeV\n";
    os << "        * etcone20/pT > 0.03\n";
    if (m_workingPoint == "AntiMuon_nominal") {
      os << "        * ptvarcone40/pT <= 0.1\n";
    }
    else if (m_workingPoint == "AntiMuon_shapeSyst1") {
      os << "        * ptvarcone40/pT <= 0.05\n";
    }
    else if (m_workingPoint == "AntiMuon_shapeSyst2") {
      os << "        * 0.05 < ptvarcone40/pT <= 0.1\n";
    }
  }

  StandardIsolation::StandardIsolation(const std::string& tightLeptonIsolation,
                                       const std::string& looseLeptonIsolation) :
    m_tightLeptonDecoration("AnalysisTop_Isol_" + tightLeptonIsolation),
    m_looseLeptonDecoration("AnalysisTop_Isol_" + looseLeptonIsolation),
    m_tightLeptonIsolation(tightLeptonIsolation),
    m_looseLeptonIsolation(looseLeptonIsolation),
    m_doTightIsolation(true),
    m_doLooseIsolation(true),
    m_doTightPromptLeptonIso(false),
    m_doLoosePromptLeptonIso(false),
    m_skipUnavailable(!ConfigurationSettings::get()->feature("AbortOnUnavailableIsolation")) {
    if (tightLeptonIsolation == "PromptLepton") m_doTightPromptLeptonIso = true;
    if (looseLeptonIsolation == "PromptLepton") m_doLoosePromptLeptonIso = true;

    if (tightLeptonIsolation == "None") m_doTightIsolation = false;

    if (looseLeptonIsolation == "None") m_doLooseIsolation = false;
    
    if ( tightLeptonIsolation == "AntiMuon_nominal"
      || tightLeptonIsolation == "AntiMuon_shapeSyst1"
      || tightLeptonIsolation == "AntiMuon_shapeSyst2" ) {
      ATH_MSG_ERROR("Cannot use isolation working point "
          << tightLeptonIsolation
          << " which is suitable for AntiMuons only.");
      throw std::runtime_error("Attempt to use inappropriate isolation working point, suitable for AntiMuons only");
    }
    if ( looseLeptonIsolation == "AntiMuon_nominal"
      || looseLeptonIsolation == "AntiMuon_shapeSyst1"
      || looseLeptonIsolation == "AntiMuon_shapeSyst2" ) {
      ATH_MSG_ERROR("Cannot use isolation working point "
          << looseLeptonIsolation
          << " which is suitable for AntiMuons only.");
      throw std::runtime_error("Attempt to use inappropriate isolation working point, suitable for AntiMuons only");
    }
  }

  bool StandardIsolation::passSelection(const xAOD::IParticle& p) const {
    // Are we doing isolation?
    if (!m_doTightIsolation) return true;

    // If we get this far then we are doing isolation
    if (p.type() == xAOD::Type::Photon ||
        p.type() == xAOD::Type::Electron ||
        p.type() == xAOD::Type::Muon) {
      if (!m_doTightPromptLeptonIso) {
        if (!m_skipUnavailable || p.isAvailable<char>(m_tightLeptonDecoration)) {
          if (p.auxdataConst<char>(m_tightLeptonDecoration) == 1) {
            return true;
          }
        }
      } else {
        // Hardcoded a bit - With PLI we need to check that it passes Loose isolation AND passes the BDT cut
        if (!m_skipUnavailable ||
            (p.isAvailable<char>(m_tightLeptonDecoration) && p.isAvailable<char>("AnalysisTop_Isol_Loose"))) {
          if (p.auxdataConst<char>(m_tightLeptonDecoration) == 1 &&
              p.auxdataConst<char>("AnalysisTop_Isol_Loose") == 1) {
            return true;
          }
        }
      }
    }

    // Not a photon, electron or muon?
    // Didn't pass isolation?
    return false;
  }

  bool StandardIsolation::passSelectionLoose(const xAOD::IParticle& p) const {
    // Are we doing isolation?
    if (!m_doLooseIsolation) return true;

    // If we get this far then we are doing isolation
    if (p.type() == xAOD::Type::Photon ||
        p.type() == xAOD::Type::Electron ||
        p.type() == xAOD::Type::Muon) {
      if (!m_doLoosePromptLeptonIso) {
        if (!m_skipUnavailable || p.isAvailable<char>(m_looseLeptonDecoration)) {
          if (p.auxdataConst<char>(m_looseLeptonDecoration) == 1) {
            return true;
          }
        }
      } else {
        // Hardcoded a bit - With PLI we need to check that it passes Loose isolation AND passes the BDT cut
        if (!m_skipUnavailable ||
            (p.isAvailable<char>(m_looseLeptonDecoration) && p.isAvailable<char>("AnalysisTop_Isol_Loose"))) {
          if (p.auxdataConst<char>(m_looseLeptonDecoration) == 1 &&
              p.auxdataConst<char>("AnalysisTop_Isol_Loose") == 1) {
            return true;
          }
        }
      }
    }

    // Not a photon, electron or muon?
    // Didn't pass isolation?
    return false;
  }

  void StandardIsolation::print(std::ostream& os) const {
    os << "    * Standard Isolation\n";
    os << "        * Tight lepton isolation = " << m_tightLeptonDecoration << "\n";
    os << "        * Loose lepton isolation = " << m_looseLeptonDecoration << "\n";
  }
}
