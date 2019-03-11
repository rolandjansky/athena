/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h"

NSubjettinessRatiosTool::NSubjettinessRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int NSubjettinessRatiosTool::modifyJet(xAOD::Jet &jet) const {
  
  if (!jet.isAvailable<float>(m_prefix+"Tau1") || 
      !jet.isAvailable<float>(m_prefix+"Tau2") ||
      !jet.isAvailable<float>(m_prefix+"Tau3") ||
      !jet.isAvailable<float>(m_prefix+"Tau4") ||
      !jet.isAvailable<float>(m_prefix+"Tau1_wta") || 
      !jet.isAvailable<float>(m_prefix+"Tau2_wta") ||
      !jet.isAvailable<float>(m_prefix+"Tau3_wta") ||
      !jet.isAvailable<float>(m_prefix+"Tau4_wta")) {
    
    ATH_MSG_WARNING("Tau decorations for "<<m_prefix<<"are not all available. Exiting..");
    return 1;
  }
  
  // Regular
  float tau1 = jet.getAttribute<float>(m_prefix+"Tau1");
  float tau2 = jet.getAttribute<float>(m_prefix+"Tau2");
  float tau3 = jet.getAttribute<float>(m_prefix+"Tau3");
  float tau4 = jet.getAttribute<float>(m_prefix+"Tau4");

  //Prevent div-0 and check against default value (-999) of the decoration
  if(tau1 > 1e-8)
    jet.setAttribute(m_prefix+"Tau21", tau2/tau1);
  else
    jet.setAttribute(m_prefix+"Tau21", -999.0);
  //Prevent div-0 and check against default value (-999) of the decoration    
  if(tau2 > 1e-8) { 
    jet.setAttribute(m_prefix+"Tau32", tau3/tau2);
    jet.setAttribute(m_prefix+"Tau42", tau4/tau2);
  }
  else {
    jet.setAttribute(m_prefix+"Tau32", -999.0);
    jet.setAttribute(m_prefix+"Tau42", -999.0);
  }
  
  float tau1_wta = jet.getAttribute<float>(m_prefix+"Tau1_wta");
  float tau2_wta = jet.getAttribute<float>(m_prefix+"Tau2_wta");
  float tau3_wta = jet.getAttribute<float>(m_prefix+"Tau3_wta");
  float tau4_wta = jet.getAttribute<float>(m_prefix+"Tau4_wta");
  
  // WTA
  //Prevent div-0 and check against default value (-999) of the decoration
  if(tau1_wta > 1e-8) 
    jet.setAttribute(m_prefix+"Tau21_wta", tau2_wta/tau1_wta);
  else
    jet.setAttribute(m_prefix+"Tau21_wta", -999.0);
  //Prevent div-0 and check against default value (-999) of the decoration
  if(tau2_wta > 1e-8) {
    jet.setAttribute(m_prefix+"Tau32_wta", tau3_wta/tau2_wta);
    jet.setAttribute(m_prefix+"Tau42_wta", tau4_wta/tau2_wta);
  }
  else {
    jet.setAttribute(m_prefix+"Tau32_wta", -999.0);
    jet.setAttribute(m_prefix+"Tau42_wta", -999.0);
  }
  
  return 0;
}
