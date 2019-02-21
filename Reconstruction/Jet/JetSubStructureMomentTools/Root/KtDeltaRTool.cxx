/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// KtDeltaRTool.cxx

#include "JetSubStructureMomentTools/KtDeltaRTool.h"
#include "JetSubStructureUtils/KtDeltaR.h"

KtDeltaRTool::KtDeltaRTool(const std::string& myname)
: JetSubStructureMomentToolsBase(myname) {
  declareProperty("JetRadius", m_jetrad =0.4);
}

int KtDeltaRTool::modifyJet(xAOD::Jet& injet) const {

  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  float ktdr_value = -999;

  if (decorate) {
    JetSubStructureUtils::KtDeltaR ktdr(m_jetrad);
    ktdr_value = ktdr.result(jet);
  }

  injet.setAttribute(m_prefix+"KtDR",ktdr_value);
    
  return 0;
}

void KtDeltaRTool::print() const {
  ATH_MSG_INFO("Reclustering size parameter: " << m_jetrad);
}

