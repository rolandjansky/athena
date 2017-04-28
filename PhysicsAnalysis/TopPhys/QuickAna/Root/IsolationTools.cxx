/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <QuickAna/IsolationTools.h>

#include <QuickAna/MessageCheck.h>

namespace ana {

IsolationBase::IsolationBase() {
}

IsolationBase::~IsolationBase() {
}


AbsoluteIsolationDC14::AbsoluteIsolationDC14(const xAOD::Iso::IsolationType type, double cutvalue) :
        m_type(type),
        m_cutvalue(cutvalue) {
}

bool AbsoluteIsolationDC14::passSelection(const xAOD::IParticle& p) const {
  using namespace msgIsolationTools;

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

    ANA_MSG_FATAL ("IsolationDC14::Not an electron or muon");
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

  using namespace msgIsolationTools;

    if (size != 20 && size != 30 && size != 40) {
      ANA_MSG_FATAL ("ptvarcone only supports cone sizes of 20, 30, 40");
      ANA_MSG_FATAL ("and you asked for " << size << ". why?");
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
    if (m_fractionLoose<0) return true;
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
    if (m_fractionLoose<0) return true;
    return (p.auxdataConst<float>("miniIso") / p.pt() < m_fractionLoose);
}

void ApproxPTVarCone::print(std::ostream& os) const {
    os << "    * Approximate Mini Isolation\n";
    os << "        * iso/pT > " << m_fraction << "\n";
}



StandardIsolation::StandardIsolation(const std::string& obj_type_str) :
    m_isolationTool("CP::IsolationSelectionTool"),
    m_isolationToolLoose("CP::IsolationSelectionToolLoose")
{
  using namespace msgIsolationTools;

    if ( ! m_isolationTool.setProperty(obj_type_str+"WP","Gradient") )
    {
      ANA_MSG_FATAL ("Failed to set working point of IsolationSelectionTool");
      exit(1);
    }
    if ( ! m_isolationTool.initialize() )
    {
      ANA_MSG_FATAL ("Failed to initialize IsolationSelectionTool");
      exit(1);
    }

    // Loose photons, electrons & muons
    if ( ! m_isolationToolLoose.setProperty(obj_type_str+"WP","LooseTrackOnly") )
    {
      ANA_MSG_ERROR ("Failed to set working point of Loose IsolationSelectionTool");
    }
    if ( ! m_isolationToolLoose.initialize() )
    {
      ANA_MSG_ERROR ("Failed to initialize Loose IsolationSelectionTool");
    }
}

bool StandardIsolation::passSelection(const xAOD::IParticle& p) const
{
  if( p.type() == xAOD::Type::Photon ){
    const xAOD::Photon& ph = dynamic_cast<const xAOD::Photon&>(p);
    return m_isolationTool.accept(ph);
  }

  if( p.type() == xAOD::Type::Electron ){
    const xAOD::Electron& el = dynamic_cast<const xAOD::Electron&>(p);
    return m_isolationTool.accept(el);
  }

  if( p.type() == xAOD::Type::Muon ){
    const xAOD::Muon& mu = dynamic_cast<const xAOD::Muon&>(p);
    return m_isolationTool.accept(mu);
  }

  // Not a photon, electron or muon?
  return false;
}

bool StandardIsolation::passSelectionLoose(const xAOD::IParticle& p) const
{
  if( p.type() == xAOD::Type::Photon ){
    const xAOD::Photon& ph = dynamic_cast<const xAOD::Photon&>(p);
    return m_isolationToolLoose.accept(ph);
  }

  if( p.type() == xAOD::Type::Electron ){
    const xAOD::Electron& el = dynamic_cast<const xAOD::Electron&>(p);
    return m_isolationToolLoose.accept(el);
  }

  if( p.type() == xAOD::Type::Muon ){
    const xAOD::Muon& mu = dynamic_cast<const xAOD::Muon&>(p);
    return m_isolationToolLoose.accept(mu);
  }

  // Not a photon, electron or muon?
  return false;
}

void StandardIsolation::print(std::ostream& os) const {
    os << "    * Standard Isolation\n";
    os << "        * Tight: Gradient Isolation\n";
    os << "        * Loose: LooseTrackOnly Isolation\n";
}

}
