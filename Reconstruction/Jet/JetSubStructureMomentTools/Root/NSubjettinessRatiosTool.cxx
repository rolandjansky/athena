/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h"

NSubjettinessRatiosTool::NSubjettinessRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("DoDichroic", m_doDichroic = false);
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
    
    ATH_MSG_ERROR("Tau decorations for " << m_prefix << " are not all available. Exiting..");
    return 1;
  }

  if (m_doDichroic) { 
    if (!jet.isAvailable<float>(m_prefix+"Tau2_ungroomed") ||
        !jet.isAvailable<float>(m_prefix+"Tau3_ungroomed") ||
        !jet.isAvailable<float>(m_prefix+"Tau4_ungroomed") ||
        !jet.isAvailable<float>(m_prefix+"Tau2_wta_ungroomed") ||
        !jet.isAvailable<float>(m_prefix+"Tau3_wta_ungroomed") ||
        !jet.isAvailable<float>(m_prefix+"Tau4_wta_ungroomed")) {

      ATH_MSG_ERROR("Ungroomed tau decorations for " << m_prefix << " are not all available. Exiting..");
      return 1;
    }
  }

  // Regular
  float tau1 = jet.getAttribute<float>(m_prefix+"Tau1");
  float tau2 = jet.getAttribute<float>(m_prefix+"Tau2");
  float tau3 = jet.getAttribute<float>(m_prefix+"Tau3");
  float tau4 = jet.getAttribute<float>(m_prefix+"Tau4");

  float tau2_ungroomed = -999.0;
  float tau3_ungroomed = -999.0;
  float tau4_ungroomed = -999.0;

  if (m_doDichroic) {
    tau2_ungroomed = jet.getAttribute<float>(m_prefix+"Tau2_ungroomed");
    tau3_ungroomed = jet.getAttribute<float>(m_prefix+"Tau3_ungroomed");
    tau4_ungroomed = jet.getAttribute<float>(m_prefix+"Tau4_ungroomed");
  }

  //Prevent div-0 and check against default value (-999) of the decoration
  if(tau1 > 1e-8) {
    jet.setAttribute(m_prefix+"Tau21", tau2/tau1);
    if(tau2_ungroomed > 1e-8) 
      jet.setAttribute(m_prefix+"Tau21_dichroic", tau2_ungroomed/tau1);
    else
      jet.setAttribute(m_prefix+"Tau21_dichroic", -999.0);
  }
  else {
    jet.setAttribute(m_prefix+"Tau21", -999.0);
    jet.setAttribute(m_prefix+"Tau21_dichroic", -999.0);
  }

  //Prevent div-0 and check against default value (-999) of the decoration    
  if(tau2 > 1e-8) { 
    jet.setAttribute(m_prefix+"Tau32", tau3/tau2);
    jet.setAttribute(m_prefix+"Tau42", tau4/tau2);

    if(tau3_ungroomed > 1e-8) 
      jet.setAttribute(m_prefix+"Tau32_dichroic", tau3_ungroomed/tau2);
    else
      jet.setAttribute(m_prefix+"Tau32_dichroic", -999.0);

    if(tau4_ungroomed > 1e-8) 
      jet.setAttribute(m_prefix+"Tau42_dichroic", tau4_ungroomed/tau2);
    else
      jet.setAttribute(m_prefix+"Tau42_dichroic", -999.0);
  }
  else {
    jet.setAttribute(m_prefix+"Tau32", -999.0);
    jet.setAttribute(m_prefix+"Tau42", -999.0);

    jet.setAttribute(m_prefix+"Tau32_dichroic", -999.0);
    jet.setAttribute(m_prefix+"Tau42_dichroic", -999.0);
  }

  float tau1_wta = jet.getAttribute<float>(m_prefix+"Tau1_wta");
  float tau2_wta = jet.getAttribute<float>(m_prefix+"Tau2_wta");
  float tau3_wta = jet.getAttribute<float>(m_prefix+"Tau3_wta");
  float tau4_wta = jet.getAttribute<float>(m_prefix+"Tau4_wta");

  float tau2_wta_ungroomed = -999.0;
  float tau3_wta_ungroomed = -999.0;
  float tau4_wta_ungroomed = -999.0;

  if (m_doDichroic) {
    tau2_wta_ungroomed = jet.getAttribute<float>(m_prefix+"Tau2_wta_ungroomed");
    tau3_wta_ungroomed = jet.getAttribute<float>(m_prefix+"Tau3_wta_ungroomed");
    tau4_wta_ungroomed = jet.getAttribute<float>(m_prefix+"Tau4_wta_ungroomed");
  }

  // WTA
  //Prevent div-0 and check against default value (-999) of the decoration
  if(tau1_wta > 1e-8) { 
    jet.setAttribute(m_prefix+"Tau21_wta", tau2_wta/tau1_wta);
    if(tau2_wta_ungroomed > 1e-8) 
      jet.setAttribute(m_prefix+"Tau21_wta_dichroic", tau2_wta_ungroomed/tau1_wta);
    else
      jet.setAttribute(m_prefix+"Tau21_wta_dichroic", -999.0);
  }
  else {
    jet.setAttribute(m_prefix+"Tau21_wta", -999.0);
    jet.setAttribute(m_prefix+"Tau21_wta_dichroic", -999.0);
  }

  //Prevent div-0 and check against default value (-999) of the decoration
  if(tau2_wta > 1e-8) {
    jet.setAttribute(m_prefix+"Tau32_wta", tau3_wta/tau2_wta);
    jet.setAttribute(m_prefix+"Tau42_wta", tau4_wta/tau2_wta);

    if(tau3_wta_ungroomed > 1e-8) 
      jet.setAttribute(m_prefix+"Tau32_wta_dichroic", tau3_wta_ungroomed/tau2_wta);
    else
      jet.setAttribute(m_prefix+"Tau32_wta_dichroic", -999.0);

    if(tau4_wta_ungroomed > 1e-8) 
      jet.setAttribute(m_prefix+"Tau42_wta_dichroic", tau4_wta_ungroomed/tau2_wta);
    else
      jet.setAttribute(m_prefix+"Tau42_wta_dichroic", -999.0);
  }
  else {
    jet.setAttribute(m_prefix+"Tau32_wta", -999.0);
    jet.setAttribute(m_prefix+"Tau42_wta", -999.0);

    jet.setAttribute(m_prefix+"Tau32_wta_dichroic", -999.0);
    jet.setAttribute(m_prefix+"Tau42_wta_dichroic", -999.0);
  }

  return 0;
}
