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

#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h" 

using fastjet::PseudoJet;

EnergyCorrelatorTool::EnergyCorrelatorTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing EnergyCorrelator tool.");
  declareProperty("Beta", m_Beta = 1.0);
  declareProperty("BetaList", m_betaVals = {});
  declareProperty("DoC3", m_doC3 = false);
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode EnergyCorrelatorTool::initialize() {
  ATH_MSG_INFO("Initializing EnergyCorrelatorTool");
  
  // Add beta = 1.0 by default
  betaVals.push_back(1.0);

  // Add beta = m_Beta by default to keep backwards compatibility
  if( fabs(m_Beta-1.0) > 1.0e-5 ) betaVals.push_back(m_Beta);

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

int EnergyCorrelatorTool::modifyJet(xAOD::Jet &injet) const {
  
  PseudoJet jet;
  PseudoJet jet_ungroomed;

  bool decorate = SetupDecoration(jet,injet);
  bool decorate_ungroomed = false;
  if(m_doDichroic) {
    // Get parent jet here and replace injet
    ElementLink<xAOD::JetContainer> parentLink = injet.auxdata<ElementLink<xAOD::JetContainer> >("Parent");

    // Return error is parent element link is broken
    if(!parentLink.isValid()) {
      ATH_MSG_ERROR("Parent element link is not valid. Aborting");
      return 1;
    }

    const xAOD::Jet* parentJet = *(parentLink);
    decorate_ungroomed = SetupDecoration(jet_ungroomed,*parentJet);
  }

  for(float beta : betaVals) {

    if(beta < 0.0) {
      ATH_MSG_WARNING("Negative beta values are not supported. Skipping " << beta);
      continue;
    }

    std::string suffix = GetBetaSuffix(beta);

    float result_ECF1 = -999;
    float result_ECF2 = -999;
    float result_ECF3 = -999;
    float result_ECF4 = -999;

    float result_ECF1_ungroomed = -999;
    float result_ECF2_ungroomed = -999;
    float result_ECF3_ungroomed = -999;

    if(decorate) {

      JetSubStructureUtils::EnergyCorrelator ECF1(1, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      JetSubStructureUtils::EnergyCorrelator ECF2(2, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      JetSubStructureUtils::EnergyCorrelator ECF3(3, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);

      result_ECF1 = ECF1.result(jet);
      result_ECF2 = ECF2.result(jet);
      result_ECF3 = ECF3.result(jet);

      if(m_doC3) {
        JetSubStructureUtils::EnergyCorrelator ECF4(4, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
        result_ECF4 = ECF4.result(jet);
      }
    
      if(decorate_ungroomed) {
        result_ECF1_ungroomed = ECF1.result(jet_ungroomed);
        result_ECF2_ungroomed = ECF2.result(jet_ungroomed);
        result_ECF3_ungroomed = ECF3.result(jet_ungroomed);
      }

    }

    injet.setAttribute(m_prefix+"ECF1"+suffix, result_ECF1);
    injet.setAttribute(m_prefix+"ECF2"+suffix, result_ECF2);
    injet.setAttribute(m_prefix+"ECF3"+suffix, result_ECF3);
    injet.setAttribute(m_prefix+"ECF4"+suffix, result_ECF4);
    
    injet.setAttribute(m_prefix+"ECF1_ungroomed"+suffix, result_ECF1_ungroomed);
    injet.setAttribute(m_prefix+"ECF2_ungroomed"+suffix, result_ECF2_ungroomed);
    injet.setAttribute(m_prefix+"ECF3_ungroomed"+suffix, result_ECF3_ungroomed);

  }

  return 0;
}

