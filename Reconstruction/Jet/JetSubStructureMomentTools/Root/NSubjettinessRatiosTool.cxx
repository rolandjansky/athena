/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h"

using namespace std;

NSubjettinessRatiosTool::NSubjettinessRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int NSubjettinessRatiosTool::modifyJet(xAOD::Jet &jet) const {
  // Regular
  float tau1 = jet.getAttribute<float>("Tau1");
  float tau2 = jet.getAttribute<float>("Tau2");
  float tau3 = jet.getAttribute<float>("Tau3");

  if(fabs(tau1) > 1e-8) // Prevent div-0
    jet.setAttribute("Tau21", tau2/tau1);
  else
    jet.setAttribute("Tau21", -999.0);
  if(fabs(tau2) > 1e-8) // Prevent div-0
    jet.setAttribute("Tau32", tau3/tau2);
  else
    jet.setAttribute("Tau32", -999.0);

  float tau1_wta = jet.getAttribute<float>("Tau1_wta");
  float tau2_wta = jet.getAttribute<float>("Tau2_wta");
  float tau3_wta = jet.getAttribute<float>("Tau3_wta");

  // WTA
  if(fabs(tau1_wta) > 1e-8) // Prevent div-0
    jet.setAttribute("Tau21_wta", tau2_wta/tau1_wta);
  else
    jet.setAttribute("Tau21_wta", -999.0);
  if(fabs(tau2_wta) > 1e-8) // Prevent div-0
    jet.setAttribute("Tau32_wta", tau3_wta/tau2_wta);
  else
    jet.setAttribute("Tau32_wta", -999.0);

  return 0;
}
