/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h" 
#include "JetSubStructureUtils/EnergyCorrelator.h" 

EnergyCorrelatorGeneralizedTool::EnergyCorrelatorGeneralizedTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Beta", m_Beta = 1.0);
  declareProperty("BetaList", m_rawBetaVals = {});
  declareProperty("DoN3", m_doN3 = false);
  declareProperty("DoLSeries", m_doLSeries = false);
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode EnergyCorrelatorGeneralizedTool::initialize() {

  // Add beta = 1.0 by default
  m_betaVals.push_back(1.0);

  // Add beta = m_Beta by default to keep backwards compatibility
  if( std::abs(m_Beta-1.0) > 1.0e-5 ) m_betaVals.push_back(m_Beta);

  // Clean up input list of beta values
  for(float beta : m_rawBetaVals) {

    // Round to the nearest 0.1
    float betaFix = round( beta * 10.0 ) / 10.0;
    if(std::abs(beta-betaFix) > 1.0e-5) ATH_MSG_DEBUG("beta = " << beta << " has been rounded to " << betaFix);

    // Skip negative values of beta
    if(betaFix < 0.0) {
      ATH_MSG_WARNING("beta must be positive. Skipping beta = " << beta);
      continue;
    }

    // Only store value if it is not already in the list
    if( std::find(m_betaVals.begin(), m_betaVals.end(), betaFix) == m_betaVals.end() ) m_betaVals.push_back(betaFix);
  }

  for(float beta : m_betaVals) {
    ATH_MSG_DEBUG("Including beta = " << beta);
  }

  ATH_CHECK(JetSubStructureMomentToolsBase::initialize());

  return StatusCode::SUCCESS;
}

int EnergyCorrelatorGeneralizedTool::modifyJet(xAOD::Jet &injet) const {
  
  fastjet::PseudoJet jet;
  fastjet::PseudoJet jet_ungroomed;
  
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

  for(float beta : m_betaVals) {

    std::string suffix = GetBetaSuffix(beta);

    // These are used for N2 and M2
    float ECFG_2_1_value = -999, ECFG_3_2_value = -999;
    float ECFG_2_1_value_ungroomed = -999, ECFG_3_2_value_ungroomed = -999;

    // These are used for N3
    float ECFG_3_1_value = -999, ECFG_4_2_value = -999;
    float ECFG_3_1_value_ungroomed = -999;

    if (decorate) {

      // These are used for N2 and M2

      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2(2, 3, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1(1, 2, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_2_1_value = ECFG_2_1.result(jet);
      ECFG_3_2_value = ECFG_3_2.result(jet);

      if (decorate_ungroomed) {
        ECFG_2_1_value_ungroomed = ECFG_2_1.result(jet_ungroomed);
        ECFG_3_2_value_ungroomed = ECFG_3_2.result(jet_ungroomed);
      }

      // These are used for N3

      if(m_doN3) {
        JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2(2, 4, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
        JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1(1, 3, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
        ECFG_3_1_value = ECFG_3_1.result(jet);
        ECFG_4_2_value = ECFG_4_2.result(jet);

        if (decorate_ungroomed) {
          ECFG_3_1_value_ungroomed = ECFG_3_1.result(jet_ungroomed);
        }

      }

    }

    injet.setAttribute(m_prefix+"ECFG_2_1"+suffix, ECFG_2_1_value);
    injet.setAttribute(m_prefix+"ECFG_3_2"+suffix, ECFG_3_2_value);
    injet.setAttribute(m_prefix+"ECFG_3_1"+suffix, ECFG_3_1_value);
    injet.setAttribute(m_prefix+"ECFG_4_2"+suffix, ECFG_4_2_value);

    injet.setAttribute(m_prefix+"ECFG_2_1_ungroomed"+suffix, ECFG_2_1_value_ungroomed);
    injet.setAttribute(m_prefix+"ECFG_3_2_ungroomed"+suffix, ECFG_3_2_value_ungroomed);
    injet.setAttribute(m_prefix+"ECFG_3_1_ungroomed"+suffix, ECFG_3_1_value_ungroomed);

  }

  // These are for t/H discrimination
  float ECFG_3_3_2_value = -999, ECFG_3_2_2_value = -999, ECFG_3_3_1_value = -999,
        ECFG_4_2_2_value = -999, ECFG_4_4_1_value = -999, ECFG_2_1_2_value = -999, ECFG_3_2_1_value =  -999,
        ECFG_3_1_1_value = -999;
  // N.B. ECFG_angles_n_beta !!

  if (decorate) {

    // These are for t/H discrimination

    if(m_doLSeries) {
      // 332
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_3_2(3, 3, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_3_2_value = ECFG_3_3_2.result(jet);

      // 322
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2_2(2, 3, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_2_2_value =  ECFG_3_2_2.result(jet);

      // 331
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_3_1(3, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_3_1_value = ECFG_3_3_1.result(jet);

      // 422
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2_2(2, 4, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_4_2_2_value = ECFG_4_2_2.result(jet);

      // 441
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_4_1(4, 4, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_4_4_1_value = ECFG_4_4_1.result(jet);

      // 212
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1_2(1, 2, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_2_1_2_value = ECFG_2_1_2.result(jet);

      // 321
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2_1(2, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_2_1_value = ECFG_3_2_1.result(jet);

      // 311
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1_1(1, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_1_1_value = ECFG_3_1_1.result(jet);
    }

  }

  injet.setAttribute(m_prefix+"ECFG_3_3_2", ECFG_3_3_2_value);
  injet.setAttribute(m_prefix+"ECFG_3_2_2", ECFG_3_2_2_value);
  injet.setAttribute(m_prefix+"ECFG_3_3_1", ECFG_3_3_1_value);
  injet.setAttribute(m_prefix+"ECFG_4_2_2", ECFG_4_2_2_value);
  injet.setAttribute(m_prefix+"ECFG_4_4_1", ECFG_4_4_1_value);
  injet.setAttribute(m_prefix+"ECFG_2_1_2", ECFG_2_1_2_value);
  injet.setAttribute(m_prefix+"ECFG_3_2_1", ECFG_3_2_1_value);
  injet.setAttribute(m_prefix+"ECFG_3_1_1", ECFG_3_1_1_value);

  return 0;
}
