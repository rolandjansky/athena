/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// KtDeltaRTool.cxx

#include "JetSubStructureMomentTools/KtDeltaRTool.h"
#include "JetSubStructureUtils/KtDeltaR.h"

KtDeltaRTool::KtDeltaRTool(const std::string& myname)
: JetSubStructureMomentToolsBase(myname) {
  declareProperty("JetRadius", m_jetrad =0.4);
}

StatusCode KtDeltaRTool::initialize() {
  return StatusCode::SUCCESS;
}

int KtDeltaRTool::modifyJet(xAOD::Jet& jet) const {
  if(checkForConstituents(jet) == false) return 1;

  JetSubStructureUtils::KtDeltaR ktdr(m_jetrad);
  jet.setAttribute("KtDR", ktdr.result(jet));
  return 0;
}

void KtDeltaRTool::print() const {
  ATH_MSG_INFO("Reclustering size parameter: " << m_jetrad);
}

