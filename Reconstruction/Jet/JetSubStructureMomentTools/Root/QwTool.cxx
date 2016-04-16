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

int QwTool::modifyJet(xAOD::Jet& jet) const {
  //if(checkForConstituents(jet) == false) return 1;

  static JetSubStructureUtils::Qw qw;
  jet.setAttribute("Qw", qw.result(jet));
  return 0;
}
