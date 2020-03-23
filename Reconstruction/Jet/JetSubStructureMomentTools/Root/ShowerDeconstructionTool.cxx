/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/ShowerDeconstructionTool.h"
//#include "JetSubStructureUtils/ShowerDeconstruction.h" 

ShowerDeconstructionTool::ShowerDeconstructionTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int ShowerDeconstructionTool::modifyJet(xAOD::Jet &/*jet*/) const {
/*
  if(checkForConstituents(jet) == false) return 1;

	double val;

  JetSubStructureUtils::ShowerDeconstruction sd_w(JetSubStructureUtils::ShowerDeconstruction::WDecayModel);
  val = sd_w.result(jet);
  ATH_MSG_VERBOSE("Adding jet ShowerDeconstructionW: " << val);
  jet.setAttribute("ShowerDeconstructionW", val);

  JetSubStructureUtils::ShowerDeconstruction sd_top(JetSubStructureUtils::ShowerDeconstruction::TopGluonModel);
  val = sd_top.result(jet);
  ATH_MSG_VERBOSE("Adding jet ShowerDeconstructionTop: " << val);
  jet.setAttribute("ShowerDeconstructionTop", val);
*/

  return 0;
}
