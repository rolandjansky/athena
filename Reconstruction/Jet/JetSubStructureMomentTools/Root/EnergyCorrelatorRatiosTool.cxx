/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h" 
#include <math.h>

using namespace std;
using fastjet::PseudoJet;

EnergyCorrelatorRatiosTool::EnergyCorrelatorRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int EnergyCorrelatorRatiosTool::modifyJet(xAOD::Jet &jet) const {
  float ecf1 = jet.getAttribute<float>("ECF1");
  float ecf2 = jet.getAttribute<float>("ECF2");
  float ecf3 = jet.getAttribute<float>("ECF3");

  // D2
  if(fabs(ecf2) > 1e-8) // Prevent div-0
    jet.setAttribute("D2", ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0));
  else
    jet.setAttribute("D2", -999.0);

  // C1
  if(fabs(ecf1) > 1e-8) // Prevent div-0
    jet.setAttribute("C1", ecf2 / pow(ecf1, 2.0));
  else
    jet.setAttribute("C1", -999.0);

  // C2
  if(fabs(ecf2) > 1e-8) // Prevent div-0
    jet.setAttribute("C2", ecf3 * ecf1 / pow(ecf2, 2.0));
  else
    jet.setAttribute("C2", -999.0);

  return 0;
}
