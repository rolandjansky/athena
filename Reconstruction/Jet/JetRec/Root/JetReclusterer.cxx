/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetReclusterer.cxx

#include "JetRec/JetReclusterer.h"
#include "JetRec/JetFinder.h"

using fastjet::PseudoJet;
using xAOD::JetContainer;

typedef IJetFinder::NameList NameList;

//**********************************************************************

JetReclusterer::JetReclusterer(std::string name)
: AsgTool(name), m_hcr(""), m_finder("") {
  declareProperty("JetConstituentsRetriever", m_hcr);
  declareProperty("JetFinder", m_finder);
}

//**********************************************************************

JetReclusterer::~JetReclusterer() { }

//**********************************************************************

StatusCode JetReclusterer::initialize() {
  if ( m_hcr.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet constituent retriever.");
  }
  if ( m_finder.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet finder.");
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetReclusterer::groom(const xAOD::Jet& jin, xAOD::JetContainer& jets) const {
  if ( m_hcr.empty() ) {
    ATH_MSG_WARNING("Jet does not have a jet constituent retriever.");
    return 2;
  }
  PseudoJetVector inps;
  NameList ghostlabs;
  m_hcr->constituents(jin, inps, &ghostlabs);
  if ( inps.size() == 0 ) {
    ATH_MSG_WARNING("Jet has no constituents.");
    return 3;
  }
  // Cluster.
  xAOD::JetInput::Type intype = jin.getInputType();
  m_finder->find(inps, jets, intype, ghostlabs);
  ATH_MSG_DEBUG("Jet count after reclustering: " << jets.size());
  return 0;
}

//**********************************************************************

void JetReclusterer::print() const {
  ATH_MSG_INFO("  Jet finder: " << m_finder.name());
}

//**********************************************************************
