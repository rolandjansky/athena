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

#include "JetSubStructureMomentTools/EnergyCorrelatorRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h" 

using fastjet::PseudoJet;

EnergyCorrelatorRatiosTool::EnergyCorrelatorRatiosTool(std::string name) :
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("BetaList", m_betaVals = {});
  declareProperty("IncludeECF4",  m_includeECF4  = false);
}

StatusCode EnergyCorrelatorRatiosTool::initialize() {
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

int EnergyCorrelatorRatiosTool::modifyJet(xAOD::Jet &jet) const {
  
  for(float beta : betaVals) {
    std::string suffix = GetBetaSuffix(beta);

    if (!jet.isAvailable<float>(m_prefix+"ECF1"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECF1" << suffix << " is not available. Exiting..");
      return 1;
    }

    if (!jet.isAvailable<float>(m_prefix+"ECF2"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECF2" << suffix << " is not available. Exiting..");
      return 1;
    }

    if (!jet.isAvailable<float>(m_prefix+"ECF3"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECF3" << suffix << " is not available. Exiting..");
      return 1;
    }

    if (m_includeECF4 && !jet.isAvailable<float>(m_prefix+"ECF4"+suffix)) {
      ATH_MSG_WARNING("Energy correlation function " << m_prefix << "ECF4" << suffix << " is not available. Exiting..");
      return 1;
    }

    float ecf1 = jet.getAttribute<float>(m_prefix+"ECF1"+suffix);
    float ecf2 = jet.getAttribute<float>(m_prefix+"ECF2"+suffix);
    float ecf3 = jet.getAttribute<float>(m_prefix+"ECF3"+suffix);

    // D2
    if(ecf2 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"D2"+suffix, ecf3 * pow(ecf1, 3.0) / pow(ecf2, 3.0));
    else
      jet.setAttribute(m_prefix+"D2"+suffix, -999.0);

    // C1
    if(ecf1 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"C1"+suffix, ecf2 / pow(ecf1, 2.0));
    else
      jet.setAttribute(m_prefix+"C1"+suffix, -999.0);

    // C2
    if(ecf2 > 1e-8) // Prevent div-0
      jet.setAttribute(m_prefix+"C2"+suffix, ecf3 * ecf1 / pow(ecf2, 2.0));
    else
      jet.setAttribute(m_prefix+"C2"+suffix, -999.0);

    if(m_includeECF4) {
      float ecf4 = jet.getAttribute<float>(m_prefix+"ECF4"+suffix);
      // C3
      if(ecf3 > 1e-8) // Prevent div-0
        jet.setAttribute(m_prefix+"C3"+suffix, ecf4 * ecf2 / pow(ecf3, 2.0));
      else
        jet.setAttribute(m_prefix+"C3"+suffix, -999.0);
    }
  }

  return 0;
}
