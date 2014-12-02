/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/KtMassDropTool.h"
#include "JetSubStructureUtils/KtMassDrop.h"

using namespace std;

KtMassDropTool::KtMassDropTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing KtMassDrop tool.");
}

int KtMassDropTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  JetSubStructureUtils::KtMassDrop ktmassdrop;
  double val = ktmassdrop.result(jet);
  ATH_MSG_VERBOSE("Adding jet ktMassDrop: " << val);
  jet.setAttribute("Mu12", val);
  return 0;
}
