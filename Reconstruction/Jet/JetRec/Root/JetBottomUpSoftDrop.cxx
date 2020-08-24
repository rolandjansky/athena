/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetBottomUpSoftDrop.cxx

#include "JetRec/JetBottomUpSoftDrop.h"
#include <iomanip>
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Filter.hh"
#include "JetEDM/PseudoJetVector.h"


using std::setw;
using fastjet::PseudoJet;
using xAOD::JetContainer;

//**********************************************************************

JetBottomUpSoftDrop::JetBottomUpSoftDrop(std::string name)
  : AsgTool(name), m_bld("",this) {
  declareProperty("ZCut", m_zcut =0.1);
  declareProperty("Beta", m_beta =0.0);
  declareProperty("R0",   m_R0   =1.0);
  declareProperty("JetBuilder", m_bld);
}

//**********************************************************************

JetBottomUpSoftDrop::~JetBottomUpSoftDrop() {
}

//**********************************************************************

StatusCode JetBottomUpSoftDrop::initialize() {
  if ( m_zcut < 0.0 || m_zcut > 10.0 ) {
    ATH_MSG_ERROR("Invalid value for ZCut " << m_zcut);
    return StatusCode::FAILURE;
  }
  if ( m_beta < 0.0 || m_beta > 10.0 ) {
    ATH_MSG_ERROR("Invalid value for Beta " << m_beta);
    return StatusCode::FAILURE;
  }
  if ( m_R0 < 0.0 || m_R0 > 10.0 ) {
    ATH_MSG_ERROR("Invalid value for R0 " << m_R0);
    return StatusCode::FAILURE;
  }
  if ( m_bld.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet builder.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetBottomUpSoftDrop::groom(const xAOD::Jet& jin,
			       const PseudoJetContainer& pjContainer,
			       xAOD::JetContainer& jets) const {
  if ( pseudojetRetriever() == nullptr ) {
    ATH_MSG_WARNING("Pseudojet retriever is null.");
    return 1;
  }
  const PseudoJet* ppjin = pseudojetRetriever()->pseudojet(jin);
  if ( ppjin == 0 ) {
    ATH_MSG_WARNING("Jet does not have a pseudojet.");
    return 1;
  }

  ////////////////////////
  //configure bottom up soft drop tool
  //https://fastjet.hepforge.org/trac/browser/contrib/contribs/RecursiveTools/tags/2.0.0-beta1
  ////////////////////////
  fastjet::contrib::BottomUpSoftDrop softdropper(m_beta, m_zcut, m_R0);
  PseudoJet pjsoftdrop = softdropper(*ppjin);
  int npsoftdrop = pjsoftdrop.pieces().size();
  xAOD::Jet* pjet = m_bld->add(pjsoftdrop, pjContainer, jets, &jin);

  pjet->setAttribute<float>("ZCut", m_zcut);
  pjet->setAttribute<float>("SoftDropBeta", m_beta);
  pjet->setAttribute<float>("SoftDropR0", m_R0);
  pjet->setAttribute<int>("NSoftDropSubjets", npsoftdrop);

  ATH_MSG_DEBUG("Properties after softdrop:");
  ATH_MSG_DEBUG("   ncon: " << pjsoftdrop.constituents().size() << "/"
                            << ppjin->constituents().size());
  ATH_MSG_DEBUG("   nsub: " << npsoftdrop);
  if ( pjet == 0 ) {
    ATH_MSG_ERROR("Unable to add jet to container");
  } else {
    ATH_MSG_DEBUG("Added jet to container.");
  }
  return 0;
}

//**********************************************************************

void JetBottomUpSoftDrop::print() const {
  ATH_MSG_INFO("  Asymmetry measure min: " << m_zcut);
  ATH_MSG_INFO("  Angular exponent: " << m_beta);
  ATH_MSG_INFO("  Characteristic jet radius: " << m_R0);
  ATH_MSG_INFO("  Jet builder: " << m_bld.name());
}

//**********************************************************************
