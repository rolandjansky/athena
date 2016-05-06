/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetPtAssociationTool.cxx

#include "JetMomentTools/JetPtAssociationTool.h"

using std::string;
using xAOD::IParticle;
using xAOD::Jet;
using xAOD::JetContainer;
using xAOD::JetConstituent;
using xAOD::JetConstituentVector;

//**********************************************************************

JetPtAssociationTool::JetPtAssociationTool(std::string myname)
: JetModifierBase(myname) {
  declareProperty("AssociationName", m_aname);
  declareProperty("InputContainer", m_conname);
}

//**********************************************************************

StatusCode JetPtAssociationTool::initialize() {
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetPtAssociationTool::modifyJet(xAOD::Jet& jet) const {
  ATH_MSG_DEBUG("Processing jet " << jet.index());
  APVector apins;
  // Retrieve the association vector.
  if ( ! jet.getAssociatedObjects(m_aname, apins) ) {
    ATH_MSG_WARNING("Jet does not have association vector " << m_aname);
    return 1;
  }
  // Retrieve the container of jets to be matched.
  const JetContainer* pjets = evtStore()->retrieve<const JetContainer>(m_conname);
  if ( pjets == 0 ) {
    ATH_MSG_WARNING("Matching jet container not found: " << m_conname);
    return 2;
  }
  // Match associated particle to jets.
  FloatVector ptfs;
  if ( ptfrac(apins, *pjets, ptfs) ) {
    ATH_MSG_WARNING("Match of association vector to jets failed");
    return 3;
  }
  // Find the best match.
  float frac = 0.0;
  unsigned int ijet_matched = ptfs.size();
  for ( unsigned int ijet=0; ijet<ptfs.size(); ++ijet ) {
    ATH_MSG_VERBOSE("  Pt fraction[" << ijet << "]: " << ptfs[ijet]);
    if ( ptfs[ijet] > frac ) {
      frac = ptfs[ijet];
      ijet_matched = ijet;
    }
  }
  ElementLink<JetContainer> el;
  if ( ijet_matched < ptfs.size() ) {
    el = ElementLink<JetContainer>(*pjets, ijet_matched);
  }
  string sfrac = m_aname + "AssociationFraction";
  string slink = m_aname + "AssociationLink";
  ATH_MSG_DEBUG("  Setting " << sfrac << " = " << frac);
  jet.setAttribute(sfrac, frac);
  ATH_MSG_DEBUG("  Setting " << slink << " = "
                << el.dataID() << "[" << el.index() << "]");
  jet.setAttribute(slink, el);
  return 0;
}

//**********************************************************************

int JetPtAssociationTool::
ptfrac(const APVector& apins, const xAOD::JetContainer& jets, FloatVector& ptfs) const {
  ptfs.clear();
  ptfs.resize(jets.size(), 0.0);
  double ptden = 0.0;
  for ( const IParticle* ppar : apins ) {
    if ( ppar == 0 ) return -1;
    ptden += ppar->pt();
  }
  if ( ptden < 0.0 ) return -2;
  if ( ptden < 1.e-20 ) return 0;
  const double inv_ptden = 1. / ptden;
  ATH_MSG_DEBUG("Match jet count: " << jets.size());
  for ( unsigned int ijet=0; ijet<jets.size(); ++ijet ) {
    const Jet* pjet = jets[ijet];
    if ( pjet == 0 ) return -3;
    APVector apouts;
    match(apins, *pjet, apouts);
    double ptsum = 0.0;
    for ( const IParticle* ppar : apouts ) {
      if ( ppar == 0 ) return -4;
      ptsum += ppar->pt();
    }
    ptfs[ijet] = ptsum*inv_ptden;
  }
  return 0;
}

//**********************************************************************

int JetPtAssociationTool::
match(const APVector& aps, const xAOD::Jet& jet, APVector& apvs) const {
  apvs.clear();
  const JetConstituentVector cons = jet.getConstituents();
  for ( const JetConstituent* pcon : cons ) {
    const IParticle* pparcon = pcon->rawConstituent();
    for ( const IParticle* pparap : aps ) {
      if ( pparap == pparcon ) {
        apvs.push_back(pparcon);
        break;
      }
    }
  }
  return 0;
}

//**********************************************************************
