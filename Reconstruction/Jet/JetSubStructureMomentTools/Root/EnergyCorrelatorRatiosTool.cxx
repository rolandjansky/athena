/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h" 

using fastjet::PseudoJet;

EnergyCorrelatorRatiosTool::EnergyCorrelatorRatiosTool(std::string name) :
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("IncludeBeta2", m_includeBeta2 = false);
  declareProperty("IncludeECF4",  m_includeECF4  = false);
}

int EnergyCorrelatorRatiosTool::modifyJet(xAOD::Jet &jet) const {
  
  if (!jet.isAvailable<float>(m_prefix+"ECF1") || !jet.isAvailable<float>(m_prefix+"ECF2") || !jet.isAvailable<float>(m_prefix+"ECF3")) {
    ATH_MSG_WARNING("Energy correlation fractions "<<m_prefix <<"are not all available. Exiting..");
    return 1;
  }
  
  float ecf1 = jet.getAttribute<float>(m_prefix+"ECF1");
  float ecf2 = jet.getAttribute<float>(m_prefix+"ECF2");
  float ecf3 = jet.getAttribute<float>(m_prefix+"ECF3");

  // D2
  if(ecf2 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"D2", ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0));
  else
    jet.setAttribute(m_prefix+"D2", -999.0);

  // C1
  if(ecf1 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"C1", ecf2 / pow(ecf1, 2.0));
  else
    jet.setAttribute(m_prefix+"C1", -999.0);

  // C2
  if(ecf2 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"C2", ecf3 * ecf1 / pow(ecf2, 2.0));
  else
    jet.setAttribute(m_prefix+"C2", -999.0);

  if(m_includeECF4) {
    // C3
    float ecf4 = jet.getAttribute<float>(m_prefix+"ECF4");
    if(ecf3 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"C3", ecf4 * ecf2 / pow(ecf3, 2.0));
    else
      jet.setAttribute(m_prefix+"C3", -999.0);
  }

  if(m_includeBeta2) {
    
    if (!jet.isAvailable<float>(m_prefix+"ECF1_Beta2") || !jet.isAvailable<float>(m_prefix+"ECF2_Beta2") || !jet.isAvailable<float>(m_prefix+"ECF3_Beta3")) {
      ATH_MSG_WARNING("Energy correlation fractions for Beta2 are not all available. Exiting..");
    return 1;
    }
        
    float ecf1_beta2 = jet.getAttribute<float>(m_prefix+"ECF1_Beta2");
    float ecf2_beta2 = jet.getAttribute<float>(m_prefix+"ECF2_Beta2");
    float ecf3_beta2 = jet.getAttribute<float>(m_prefix+"ECF3_Beta2");
    
    // D2
    if(ecf2_beta2 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"D2_Beta2", ecf3_beta2 * pow(ecf1_beta2, 3.0) / pow(ecf2_beta2, 3.0));
    else
      jet.setAttribute(m_prefix+"D2_Beta2", -999.0);

    // C1
    if(ecf1_beta2 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"C1_Beta2", ecf2_beta2 / pow(ecf1_beta2, 2.0));
    else
      jet.setAttribute(m_prefix+"C1_Beta2", -999.0);

    // C2
    if(ecf2_beta2 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"C2_Beta2", ecf3_beta2 * ecf1_beta2 / pow(ecf2_beta2, 2.0));
    else
      jet.setAttribute(m_prefix+"C2_Beta2", -999.0);

    if(m_includeECF4) {
      float ecf4_beta2 = jet.getAttribute<float>(m_prefix+"ECF4_Beta2");
      // C3
      if(ecf3_beta2 > 1e-8) // Prevent div-0
        jet.setAttribute(m_prefix+"C3_Beta2", ecf4_beta2 * ecf2_beta2 / pow(ecf3_beta2, 2.0));
      else
        jet.setAttribute(m_prefix+"C3_Beta2", -999.0);
    }
  }

  return 0;
}
