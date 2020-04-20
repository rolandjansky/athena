/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/NSubjettinessRatiosTool.h"

NSubjettinessRatiosTool::NSubjettinessRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("AlphaList", m_rawAlphaVals = {});
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode NSubjettinessRatiosTool::initialize() {
  // Add alpha = 1.0 by default
  m_alphaVals.push_back(1.0);

  // Clean up input list of alpha values
  for(float alpha : m_rawAlphaVals) {

    // Round to the nearest 0.1
    float alphaFix = round( alpha * 10.0 ) / 10.0;
    if(std::abs(alpha-alphaFix) > 1.0e-5) ATH_MSG_DEBUG("alpha = " << alpha << " has been rounded to " << alphaFix);

    // Skip negative values of alpha
    if(alphaFix < 0.0) {
      ATH_MSG_WARNING("alpha must be positive. Skipping alpha = " << alpha);
      continue;
    }

    // Only store value if it is not already in the list
    if( std::find(m_alphaVals.begin(), m_alphaVals.end(), alphaFix) == m_alphaVals.end() ) m_alphaVals.push_back(alphaFix);
  }

  for(float alpha : m_alphaVals) {
    ATH_MSG_DEBUG("Including alpha = " << alpha);
  }

  ATH_CHECK(JetSubStructureMomentToolsBase::initialize());

  return StatusCode::SUCCESS;
}

int NSubjettinessRatiosTool::modifyJet(xAOD::Jet &jet) const {

  for(float alpha : m_alphaVals) {
    std::string suffix = GetAlphaSuffix(alpha);

    if (!jet.isAvailable<float>(m_prefix+"Tau1"+suffix) ||
        !jet.isAvailable<float>(m_prefix+"Tau2"+suffix) ||
        !jet.isAvailable<float>(m_prefix+"Tau3"+suffix) ||
        !jet.isAvailable<float>(m_prefix+"Tau4"+suffix) ||
        !jet.isAvailable<float>(m_prefix+"Tau1_wta"+suffix) ||
        !jet.isAvailable<float>(m_prefix+"Tau2_wta"+suffix) ||
        !jet.isAvailable<float>(m_prefix+"Tau3_wta"+suffix) ||
        !jet.isAvailable<float>(m_prefix+"Tau4_wta"+suffix)) {

      ATH_MSG_ERROR("Tau decorations for " << m_prefix << " with " << suffix << " are not all available. Exiting..");
      return 1;
    }

    if (m_doDichroic) {
      if (!jet.isAvailable<float>(m_prefix+"Tau2_ungroomed"+suffix) ||
          !jet.isAvailable<float>(m_prefix+"Tau3_ungroomed"+suffix) ||
          !jet.isAvailable<float>(m_prefix+"Tau4_ungroomed"+suffix) ||
          !jet.isAvailable<float>(m_prefix+"Tau2_wta_ungroomed"+suffix) ||
          !jet.isAvailable<float>(m_prefix+"Tau3_wta_ungroomed"+suffix) ||
          !jet.isAvailable<float>(m_prefix+"Tau4_wta_ungroomed"+suffix)) {

        ATH_MSG_ERROR("Ungroomed tau decorations for " << m_prefix << " with " << suffix << " are not all available. Exiting..");
        return 1;
      }
    }

    // Regular
    float tau1 = jet.getAttribute<float>(m_prefix+"Tau1"+suffix);
    float tau2 = jet.getAttribute<float>(m_prefix+"Tau2"+suffix);
    float tau3 = jet.getAttribute<float>(m_prefix+"Tau3"+suffix);
    float tau4 = jet.getAttribute<float>(m_prefix+"Tau4"+suffix);

    float tau2_ungroomed = -999.0;
    float tau3_ungroomed = -999.0;
    float tau4_ungroomed = -999.0;

    if (m_doDichroic) {
      tau2_ungroomed = jet.getAttribute<float>(m_prefix+"Tau2_ungroomed"+suffix);
      tau3_ungroomed = jet.getAttribute<float>(m_prefix+"Tau3_ungroomed"+suffix);
      tau4_ungroomed = jet.getAttribute<float>(m_prefix+"Tau4_ungroomed"+suffix);
    }

    //Prevent div-0 and check against default value (-999) of the decoration
    if(tau1 > 1e-8) {
      jet.setAttribute(m_prefix+"Tau21"+suffix, tau2/tau1);
      if(tau2_ungroomed > 1e-8)
        jet.setAttribute(m_prefix+"Tau21_dichroic"+suffix, tau2_ungroomed/tau1);
      else
        jet.setAttribute(m_prefix+"Tau21_dichroic"+suffix, -999.0);
    }
    else {
      jet.setAttribute(m_prefix+"Tau21"+suffix, -999.0);
      jet.setAttribute(m_prefix+"Tau21_dichroic"+suffix, -999.0);
    }

    //Prevent div-0 and check against default value (-999) of the decoration
    if(tau2 > 1e-8) {
      jet.setAttribute(m_prefix+"Tau32"+suffix, tau3/tau2);
      jet.setAttribute(m_prefix+"Tau42"+suffix, tau4/tau2);

      if(tau3_ungroomed > 1e-8)
        jet.setAttribute(m_prefix+"Tau32_dichroic"+suffix, tau3_ungroomed/tau2);
      else
        jet.setAttribute(m_prefix+"Tau32_dichroic"+suffix, -999.0);

      if(tau4_ungroomed > 1e-8)
        jet.setAttribute(m_prefix+"Tau42_dichroic"+suffix, tau4_ungroomed/tau2);
      else
        jet.setAttribute(m_prefix+"Tau42_dichroic"+suffix, -999.0);
    }
    else {
      jet.setAttribute(m_prefix+"Tau32"+suffix, -999.0);
      jet.setAttribute(m_prefix+"Tau42"+suffix, -999.0);

      jet.setAttribute(m_prefix+"Tau32_dichroic"+suffix, -999.0);
      jet.setAttribute(m_prefix+"Tau42_dichroic"+suffix, -999.0);
    }

    float tau1_wta = jet.getAttribute<float>(m_prefix+"Tau1_wta"+suffix);
    float tau2_wta = jet.getAttribute<float>(m_prefix+"Tau2_wta"+suffix);
    float tau3_wta = jet.getAttribute<float>(m_prefix+"Tau3_wta"+suffix);
    float tau4_wta = jet.getAttribute<float>(m_prefix+"Tau4_wta"+suffix);

    float tau2_wta_ungroomed = -999.0;
    float tau3_wta_ungroomed = -999.0;
    float tau4_wta_ungroomed = -999.0;

    if (m_doDichroic) {
      tau2_wta_ungroomed = jet.getAttribute<float>(m_prefix+"Tau2_wta_ungroomed"+suffix);
      tau3_wta_ungroomed = jet.getAttribute<float>(m_prefix+"Tau3_wta_ungroomed"+suffix);
      tau4_wta_ungroomed = jet.getAttribute<float>(m_prefix+"Tau4_wta_ungroomed"+suffix);
    }

    // WTA
    //Prevent div-0 and check against default value (-999) of the decoration
    if(tau1_wta > 1e-8) {
      jet.setAttribute(m_prefix+"Tau21_wta"+suffix, tau2_wta/tau1_wta);
      if(tau2_wta_ungroomed > 1e-8)
        jet.setAttribute(m_prefix+"Tau21_wta_dichroic"+suffix, tau2_wta_ungroomed/tau1_wta);
      else
        jet.setAttribute(m_prefix+"Tau21_wta_dichroic"+suffix, -999.0);
    }
    else {
      jet.setAttribute(m_prefix+"Tau21_wta"+suffix, -999.0);
      jet.setAttribute(m_prefix+"Tau21_wta_dichroic"+suffix, -999.0);
    }

    //Prevent div-0 and check against default value (-999) of the decoration
    if(tau2_wta > 1e-8) {
      jet.setAttribute(m_prefix+"Tau32_wta"+suffix, tau3_wta/tau2_wta);
      jet.setAttribute(m_prefix+"Tau42_wta"+suffix, tau4_wta/tau2_wta);

      if(tau3_wta_ungroomed > 1e-8)
        jet.setAttribute(m_prefix+"Tau32_wta_dichroic"+suffix, tau3_wta_ungroomed/tau2_wta);
      else
        jet.setAttribute(m_prefix+"Tau32_wta_dichroic"+suffix, -999.0);

      if(tau4_wta_ungroomed > 1e-8)
        jet.setAttribute(m_prefix+"Tau42_wta_dichroic"+suffix, tau4_wta_ungroomed/tau2_wta);
      else
        jet.setAttribute(m_prefix+"Tau42_wta_dichroic"+suffix, -999.0);
    }
    else {
      jet.setAttribute(m_prefix+"Tau32_wta"+suffix, -999.0);
      jet.setAttribute(m_prefix+"Tau42_wta"+suffix, -999.0);

      jet.setAttribute(m_prefix+"Tau32_wta_dichroic"+suffix, -999.0);
      jet.setAttribute(m_prefix+"Tau42_wta_dichroic"+suffix, -999.0);
    }
  }

  return 0;
}
