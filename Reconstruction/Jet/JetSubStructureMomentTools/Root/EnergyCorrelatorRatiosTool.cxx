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
  declareProperty("IncludeBeta2", m_includeBeta2 = false);
  declareProperty("IncludeECF4", m_includeECF4 = false);
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

  if(m_includeECF4) {
    // C3
    float ecf4 = jet.getAttribute<float>("ECF4");
    if(fabs(ecf3) > 1e-8) // Prevent div-0
      jet.setAttribute("C3", ecf4 * ecf2 / pow(ecf3, 2.0));
    else
      jet.setAttribute("C3", -999.0);
  }

  if(m_includeBeta2) {
    float ecf1_beta2 = jet.getAttribute<float>("ECF1_Beta2");
    float ecf2_beta2 = jet.getAttribute<float>("ECF2_Beta2");
    float ecf3_beta2 = jet.getAttribute<float>("ECF3_Beta2");

    // D2
    if(fabs(ecf2_beta2) > 1e-8) // Prevent div-0
      jet.setAttribute("D2_Beta2", ecf3_beta2 * pow(ecf1_beta2, 3.0) / pow(ecf2_beta2, 3.0));
    else
      jet.setAttribute("D2_Beta2", -999.0);

    // C1
    if(fabs(ecf1_beta2) > 1e-8) // Prevent div-0
      jet.setAttribute("C1_Beta2", ecf2_beta2 / pow(ecf1_beta2, 2.0));
    else
      jet.setAttribute("C1_Beta2", -999.0);

    // C2
    if(fabs(ecf2_beta2) > 1e-8) // Prevent div-0
      jet.setAttribute("C2_Beta2", ecf3_beta2 * ecf1_beta2 / pow(ecf2_beta2, 2.0));
    else
      jet.setAttribute("C2_Beta2", -999.0);

    if(m_includeECF4) {
      float ecf4_beta2 = jet.getAttribute<float>("ECF4_Beta2");
      // C3
      if(fabs(ecf3_beta2) > 1e-8) // Prevent div-0
        jet.setAttribute("C3_Beta2", ecf4_beta2 * ecf2_beta2 / pow(ecf3_beta2, 2.0));
      else
        jet.setAttribute("C3_Beta2", -999.0);
    }
  }

  return 0;
}
