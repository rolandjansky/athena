/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/PlanarFlowTool.h"
#include "JetSubStructureUtils/PlanarFlow.h" 

using namespace std;
using fastjet::PseudoJet;

PlanarFlowTool::PlanarFlowTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing PlanarFlow tool.");
}

int PlanarFlowTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  JetSubStructureUtils::PlanarFlow pf;
  double val = pf.result(jet);
  ATH_MSG_VERBOSE("Adding jet PlanarFlow: " << val);
  jet.setAttribute("PlanarFlow", val);
  return 0;
}
