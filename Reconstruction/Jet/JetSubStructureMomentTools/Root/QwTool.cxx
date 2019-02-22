/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/QwTool.h"
#include "JetSubStructureUtils/Qw.h"

QwTool::QwTool(const std::string& myname)
: JetSubStructureMomentToolsBase(myname) {
}

int QwTool::modifyJet(xAOD::Jet& injet) const {
  
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);
  float qw_value = -999;

  if (decorate) {
    static JetSubStructureUtils::Qw qw;
    qw_value = qw.result(jet);
  }
    
  injet.setAttribute(m_prefix+"Qw", qw_value);
  return 0;
}
