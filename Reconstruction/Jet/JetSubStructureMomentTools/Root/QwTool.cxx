/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/QwTool.h"
#include "JetSubStructureUtils/Qw.h"

QwTool::QwTool(const std::string& myname)
: JetSubStructureMomentToolsBase(myname) {
}

// StatusCode QwTool::initialize() {
//   return StatusCode::SUCCESS;
// }

int QwTool::modifyJet(xAOD::Jet& injet) const {
  //if(checkForConstituents(jet) == false) return 1;

  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);
  float qw_value = -999;

  if (decorate) {
    static JetSubStructureUtils::Qw qw;
    qw_value = qw.result(jet);
  }
    
  injet.setAttribute("Qw", qw_value);
  return 0;
}
