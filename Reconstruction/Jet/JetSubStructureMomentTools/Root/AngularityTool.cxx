/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/AngularityTool.h"
#include "JetSubStructureUtils/Angularity.h"

using namespace std;
using fastjet::PseudoJet;

AngularityTool::AngularityTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing angularity tool.");
}

int AngularityTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  JetSubStructureUtils::Angularity angularity;
  double val = angularity.result(jet);
  ATH_MSG_VERBOSE("Adding jet angularity: " << val);
  jet.setAttribute("Angularity", val);
  return 0;
}
