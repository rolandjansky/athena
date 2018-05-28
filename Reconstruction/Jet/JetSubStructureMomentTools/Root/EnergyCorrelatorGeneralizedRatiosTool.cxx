/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h" 
#include <math.h>

using namespace std;
using fastjet::PseudoJet;

EnergyCorrelatorGeneralizedRatiosTool::EnergyCorrelatorGeneralizedRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int EnergyCorrelatorGeneralizedRatiosTool::modifyJet(xAOD::Jet &jet) const {
  float ecfg_2_1 = jet.getAttribute<float>("ECFG_2_1");
  float ecfg_3_2 = jet.getAttribute<float>("ECFG_3_2");

  float ecfg_4_2 = jet.getAttribute<float>("ECFG_4_2");
  float ecfg_3_1 = jet.getAttribute<float>("ECFG_3_1");

  // these ones for t/H discrimination
  float ecfg_1_4 = jet.getAttribute<float>("ECFG_1_4");
  float ecfg_3_4 = jet.getAttribute<float>("ECFG_3_4");
  float ecfg_1_3 = jet.getAttribute<float>("ECFG_1_3");
  float ecfg_2_3 = jet.getAttribute<float>("ECFG_2_3");

  // N2
  if(fabs(ecfg_2_1) > 1e-8) // Prevent div-0
    jet.setAttribute("N2", ecfg_3_2  / (pow(ecfg_2_1, 2.0)));
  else
    jet.setAttribute("N2", -999.0);

  // N3
  if(fabs(ecfg_3_1) > 1e-8) // Prevent div-0
    jet.setAttribute("N3", ecfg_4_2  / (pow(ecfg_3_1, 2.0)));
  else
    jet.setAttribute("N3", -999.0);

  // M2
  if(fabs(ecfg_2_1) > 1e-8) // Prevent div-0
    jet.setAttribute("M2", ecfg_3_2  / ecfg_2_1);
  else
    jet.setAttribute("M2", -999.0);

  // L1 (experimental for ttH)
  if(fabs(ecfg_3_4) > 1e-8)
    jet.setAttribute("L1", ecfg_1_4 / pow(ecfg_3_4,2));
  else
    jet.setAttribute("L1",-999.0);

  // L2 (experimental for ttH)
  if(fabs(ecfg_2_3) > 1e-8)
    jet.setAttribute("L2", ecfg_1_3 / (pow(ecfg_2_3,(4/3))));
  else
    jet.setAttribute("L2",-999.0);
  
  return 0;
}
