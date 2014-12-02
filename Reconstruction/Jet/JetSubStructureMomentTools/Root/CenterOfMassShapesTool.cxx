/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
#include "JetSubStructureUtils/Thrust.h"
#include "JetSubStructureUtils/FoxWolfram.h"
#include "JetSubStructureUtils/SphericityTensor.h"

using namespace std;
using fastjet::PseudoJet;

CenterOfMassShapesTool::CenterOfMassShapesTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing CenterOfMassShapes tool.");
}

int CenterOfMassShapesTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  JetSubStructureUtils::Thrust t;
  JetSubStructureUtils::FoxWolfram foxwolfram;
  JetSubStructureUtils::SphericityTensor sphericity;
  map<string, double> res;
  
  res = t.result(jet);
  jet.setAttribute("ThrustMin", res["ThrustMin"]);
  jet.setAttribute("ThrustMaj", res["ThrustMaj"]);

  res = foxwolfram.result(jet);
  jet.setAttribute("FoxWolfram0", res["FoxWolfram0"]);
  jet.setAttribute("FoxWolfram1", res["FoxWolfram1"]);
  jet.setAttribute("FoxWolfram2", res["FoxWolfram2"]);
  jet.setAttribute("FoxWolfram3", res["FoxWolfram3"]);
  jet.setAttribute("FoxWolfram4", res["FoxWolfram4"]);

  res = sphericity.result(jet);
  jet.setAttribute("Sphericity", res["Sphericity"]);
  jet.setAttribute("Aplanarity", res["Aplanarity"]);

  return 0;
}

