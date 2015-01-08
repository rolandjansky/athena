/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetPseudojetRetriever.cxx

#include "JetRec/JetPseudojetRetriever.h"
#include "JetEDM/FastJetLink.h"
#include "JetEDM/PseudoJetMap.h"
#include "xAODJet/Jet_PseudoJet.icc"

using jet::PseudoJetMap;

//**********************************************************************

JetPseudojetRetriever::JetPseudojetRetriever(std::string name)
: AsgTool(name) {
  //declareProperty("Attributes", m_atts);
}

//**********************************************************************

StatusCode JetPseudojetRetriever::initialize() {
  return StatusCode::SUCCESS;
}
  

//**********************************************************************

const fastjet::PseudoJet* JetPseudojetRetriever::
pseudojet(const xAOD::Jet& jet) const {
  const fastjet::PseudoJet* ppj = nullptr;
  ppj = jet.getPseudoJet();
  if ( ppj != nullptr ) return ppj;
  PseudoJetMap* ppjmap;
  std::string pjmapname = label();
  if ( evtStore()->contains<PseudoJetMap>(pjmapname) ) {
    if ( evtStore()->retrieve(ppjmap, pjmapname).isSuccess() ) {
      PseudoJetMap::const_iterator ipj = ppjmap->find(&jet);
      if ( ipj !=  ppjmap->end() ) ppj = ipj->second;
    } else {
      ATH_MSG_WARNING("Unable to retrieve pseudojet map.");
    }
  }
  ATH_MSG_VERBOSE("Retrieved pseudojet @" << long(ppj)
                  << " for jet @" << long(&jet)
                  << " from " << pjmapname);
  return ppj;
}

//**********************************************************************

std::string JetPseudojetRetriever::label() const {
  return "JetPseudojetMap";
}

//**********************************************************************

void JetPseudojetRetriever::print() const {
  ATH_MSG_INFO("JetPseudojetRetriever");
}

//**********************************************************************
