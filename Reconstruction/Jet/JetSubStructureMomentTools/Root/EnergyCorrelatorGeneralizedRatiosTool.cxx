/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------------------------
// The default behavior of this tool is to use beta = 1.0, but multiple 
// values of beta can be used simultaneously. The property BetaList 
// should be passed a list of floats. Values of < 0 or > 10 may result
// in poblematic output variable names and all values will be rounded
// to the nearest 0.1. No suffix will be added to the outputs for beta = 1.0 
// and for other values a suffix of _BetaN will be added where N= 10*beta. 
// ----------------------------------------------------------------

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h" 

using fastjet::PseudoJet;

EnergyCorrelatorGeneralizedRatiosTool::EnergyCorrelatorGeneralizedRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("BetaList", m_betaVals = {});
}

StatusCode EnergyCorrelatorGeneralizedRatiosTool::initialize() {
  ATH_MSG_INFO("Initializing EnergyCorrelatorTool");

  // Add beta = 1.0 by default
  betaVals.push_back(1.0);

  // Clean up input list of beta values, rounding to nearest 0.1 and removing duplicates
  for(float beta : m_betaVals) {
    float betaFix = round( beta * 10.0 ) / 10.0;
    if( std::find(betaVals.begin(), betaVals.end(), betaFix) == betaVals.end() ) betaVals.push_back(betaFix);
  }

  for(float beta : betaVals) {
    ATH_MSG_DEBUG("Including beta = " << beta);
  }

  ATH_CHECK(JetSubStructureMomentToolsBase::initialize());

  return StatusCode::SUCCESS;
}

int EnergyCorrelatorGeneralizedRatiosTool::modifyJet(xAOD::Jet &jet) const {

  for(float beta : betaVals) {
    std::string suffix = GetBetaSuffix(beta);

    if (!jet.isAvailable<float>(m_prefix+"ECFG_2_1"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECFG_2_1" << suffix << "' is not available. Exiting..");
      return 1;
    }

    if (!jet.isAvailable<float>(m_prefix+"ECFG_3_1"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECFG_3_1" << suffix << "' is not available. Exiting..");
      return 1;
    }

    if (!jet.isAvailable<float>(m_prefix+"ECFG_3_2"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECFG_3_2" << suffix << "' is not available. Exiting..");
      return 1;
    }

    if (!jet.isAvailable<float>(m_prefix+"ECFG_4_2"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECFG_4_2" << suffix << "' is not available. Exiting..");
      return 1;
    }

    float ecfg_2_1 = jet.getAttribute<float>(m_prefix+"ECFG_2_1"+suffix);
    float ecfg_3_2 = jet.getAttribute<float>(m_prefix+"ECFG_3_2"+suffix);

    float ecfg_4_2 = jet.getAttribute<float>(m_prefix+"ECFG_4_2"+suffix);
    float ecfg_3_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_1"+suffix);

    // N2

    if(ecfg_2_1 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"N2"+suffix, ecfg_3_2  / (pow(ecfg_2_1, 2.0)));
    else
      jet.setAttribute(m_prefix+"N2"+suffix, -999.0);

    // N3
    if(ecfg_3_1 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"N3"+suffix, ecfg_4_2  / (pow(ecfg_3_1, 2.0)));
    else
      jet.setAttribute(m_prefix+"N3"+suffix, -999.0);

    // M2
    if(ecfg_2_1 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"M2"+suffix, ecfg_3_2  / ecfg_2_1);
    else
      jet.setAttribute(m_prefix+"M2"+suffix, -999.0);

  }

  // These are for t/H discrimination
  if (!jet.isAvailable<float>(m_prefix+"ECFG_2_1_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_1_1") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_2_1") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_2_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_4_2_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_3_1") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_4_4_1")) {
    ATH_MSG_WARNING("L series energy correlation functions with prefix '" << m_prefix << "' are not all available. Exiting..");
    return 1;
  }

  float ecfg_2_1_2 = jet.getAttribute<float>(m_prefix+"ECFG_2_1_2");
  float ecfg_3_1_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_1_1");
  float ecfg_3_2_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_2_1");  
  float ecfg_3_2_2 = jet.getAttribute<float>(m_prefix+"ECFG_3_2_2");
  float ecfg_4_2_2 = jet.getAttribute<float>(m_prefix+"ECFG_4_2_2");
  float ecfg_3_3_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_3_1");
  float ecfg_4_4_1 = jet.getAttribute<float>(m_prefix+"ECFG_4_4_1");

  // L-series variables
  // (experimental for ttH t/H discrimination)

  /*
     The exponents are determined in order to make 
     the whole thing dimensionless

     E = (a*n) / (b*m)
     for an ECFG_X_Y_Z, a=Y, n=Z

     e.g. for L1

     ecfg_3_3_1 / ecfg_2_1_2
     E = (3*1) / (1*2) = 3./2.
     */

  if(ecfg_2_1_2 > 1e-8) // Prevent div-0
  {
    jet.setAttribute(m_prefix+"L1", ecfg_3_2_1 / (pow(ecfg_2_1_2, (1.) )));
    jet.setAttribute(m_prefix+"L2", ecfg_3_3_1 / (pow(ecfg_2_1_2, (3./2.) )));
  }
  else
  {
    jet.setAttribute(m_prefix+"L1",-999.0);
    jet.setAttribute(m_prefix+"L2",-999.0);
  }

  if(ecfg_3_3_1 > 1e-8) // Prevent div-0
  {  
    jet.setAttribute(m_prefix+"L3", ecfg_3_1_1 / (pow(ecfg_3_3_1, (1./3.) )) );
    jet.setAttribute(m_prefix+"L4", ecfg_3_2_2 / (pow(ecfg_3_3_1, (4./3.) )) );
  }
  else
  {
    jet.setAttribute(m_prefix+"L3",-999.0);
    jet.setAttribute(m_prefix+"L4",-999.0);
  }

  if(ecfg_4_4_1 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"L5", ecfg_4_2_2 / (pow(ecfg_4_4_1, (1.) )) );
  else
    jet.setAttribute(m_prefix+"L5",-999.0);

  return 0;
}
