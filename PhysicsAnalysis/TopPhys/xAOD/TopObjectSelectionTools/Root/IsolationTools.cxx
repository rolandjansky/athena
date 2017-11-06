/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/IsolationTools.h"

#include "TopEvent/EventTools.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"

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

    std::cout << "IsolationDC14::Not an electron or muon" << std::endl;
    exit(1);

    //should never happen (famous last words for a comment, no?)
    return false;
}

///Implement me!
bool AbsoluteIsolationDC14::passSelectionLoose(const xAOD::IParticle& /*p*/) const {
    return true;
}

void AbsoluteIsolationDC14::print(std::ostream& os) const {
    os << "    * AbsoluteIsolationDC14\n";
    os << "        * " << m_type << " > " << m_cutvalue << " MeV\n";
    //missing in xaodprimities-00-00-02 os << "        * " << xAOD::Iso::toString(m_type) << " > " << m_cutvalue << " MeV\n";
}



RelativePTVarCone::RelativePTVarCone(unsigned int size, double fraction, unsigned int sizeLoose, double fractionLoose) :
    m_fraction(fraction),
    m_fractionLoose(fractionLoose) {

    if (size != 20 && size != 30 && size != 40) {
        std::cout << "ptvarcone only supports cone sizes of 20, 30, 40" << std::endl;
        std::cout << "and you asked for " << size << ". why?" << std::endl;
        exit(1);
    }

    std::stringstream ss;
    ss << "ptvarcone" << size;
    m_size = ss.str();

    ss.str("");
    ss << "ptvarcone" << sizeLoose;
    m_sizeLoose = ss.str();
}

bool RelativePTVarCone::passSelection(const xAOD::IParticle& p) const {
    return (p.auxdataConst<float>(m_size) / p.pt() < m_fraction);
}

bool RelativePTVarCone::passSelectionLoose(const xAOD::IParticle& p) const {
    return (p.auxdataConst<float>(m_sizeLoose) / p.pt() < m_fractionLoose);
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
    return (p.auxdataConst<float>("miniIso") / p.pt() < m_fraction);
}

bool ApproxPTVarCone::passSelectionLoose(const xAOD::IParticle& p) const {
    return (p.auxdataConst<float>("miniIso") / p.pt() < m_fractionLoose);
}

void ApproxPTVarCone::print(std::ostream& os) const {
    os << "    * Approximate Mini Isolation\n";
    os << "        * iso/pT > " << m_fraction << "\n";
}



StandardIsolation::StandardIsolation(const std::string& tightLeptonIsolation,const std::string& looseLeptonIsolation) :
  m_tightLeptonDecoration("AnalysisTop_Isol_"+tightLeptonIsolation),
  m_looseLeptonDecoration("AnalysisTop_Isol_"+looseLeptonIsolation),
  m_tightLeptonIsolation(tightLeptonIsolation),
  m_looseLeptonIsolation(looseLeptonIsolation),
  m_doTightIsolation(true),
  m_doLooseIsolation(true),
  m_doTightPromptLeptonIso(false),
  m_doLoosePromptLeptonIso(false)
{
  if (tightLeptonIsolation == "PromptLepton") m_doTightPromptLeptonIso = true;
  if (looseLeptonIsolation == "PromptLepton") m_doLoosePromptLeptonIso = true;

  if (tightLeptonIsolation == "None")
      m_doTightIsolation = false;
  
  if (looseLeptonIsolation == "None")
      m_doLooseIsolation = false;
  
}

bool StandardIsolation::passSelection(const xAOD::IParticle& p) const 
{
  // Are we doing isolation?
  if (!m_doTightIsolation)
      return true;
  
  // If we get this far then we are doing isolation
  if ( p.type() == xAOD::Type::Photon || 
       p.type() == xAOD::Type::Electron ||
       p.type() == xAOD::Type::Muon )
  {
    if(!m_doTightPromptLeptonIso){
      if (p.isAvailable<char>(m_tightLeptonDecoration)) {
	if (p.auxdataConst<char>(m_tightLeptonDecoration) == 1) {
	  return true;
	}
      }
    }
    else{
      // Hardcoded a bit - With PLI we need to check that it passes Loose isolation AND passes the BDT cut
      if (p.isAvailable<char>(m_tightLeptonDecoration) && p.isAvailable<char>("AnalysisTop_Isol_Loose")){
	if (p.auxdataConst<char>(m_tightLeptonDecoration) == 1 && p.auxdataConst<char>("AnalysisTop_Isol_Loose") == 1) {
	  return true;
	}
      }
    }    
  }
  
  // Not a photon, electron or muon?
  // Didn't pass isolation?
  return false;
}

bool StandardIsolation::passSelectionLoose(const xAOD::IParticle& p) const 
{
  // Are we doing isolation?
  if (!m_doLooseIsolation)
      return true;
  
  // If we get this far then we are doing isolation  
  if ( p.type() == xAOD::Type::Photon || 
       p.type() == xAOD::Type::Electron ||
       p.type() == xAOD::Type::Muon ) 
  {
    if(!m_doLoosePromptLeptonIso){
      if (p.isAvailable<char>(m_looseLeptonDecoration)) {
	if (p.auxdataConst<char>(m_looseLeptonDecoration) == 1) {
	  return true;
	}
      }
    }
    else{
      // Hardcoded a bit - With PLI we need to check that it passes Loose isolation AND passes the BDT cut              
      if (p.isAvailable<char>(m_looseLeptonDecoration) && p.isAvailable<char>("AnalysisTop_Isol_Loose")){
	if (p.auxdataConst<char>(m_looseLeptonDecoration) == 1 && p.auxdataConst<char>("AnalysisTop_Isol_Loose") == 1) {
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
    os << "        * Tight lepton isolation = "<<m_tightLeptonDecoration<< "\n";
    os << "        * Loose lepton isolation = "<<m_looseLeptonDecoration<< "\n";
}

}
