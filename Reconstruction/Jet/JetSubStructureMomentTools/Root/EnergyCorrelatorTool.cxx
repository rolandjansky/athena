/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h" 

EnergyCorrelatorTool::EnergyCorrelatorTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Beta", m_Beta = 1.0);
  declareProperty("BetaList", m_rawBetaVals = {});
  declareProperty("DoC3", m_doC3 = false);
  declareProperty("DoC4", m_doC4 = false);
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode EnergyCorrelatorTool::initialize() {

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

  // If DoC4 is set to true, set DoC3 to true by default since it won't
  // add any additional computational overhead
  if(m_doC4) m_doC3 = true;

  ATH_CHECK(JetSubStructureMomentToolsBase::initialize());

  return StatusCode::SUCCESS;
}

int EnergyCorrelatorTool::modifyJet(xAOD::Jet &injet) const {

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

    if(beta < 0.0) {
      ATH_MSG_WARNING("Negative beta values are not supported. Skipping " << beta);
      continue;
    }

    std::string suffix = GetBetaSuffix(beta);

    float result_ECF1 = -999;
    float result_ECF2 = -999;
    float result_ECF3 = -999;
    float result_ECF4 = -999;
    float result_ECF5 = -999;

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

      if(m_doC4) {
        JetSubStructureUtils::EnergyCorrelator ECF5(5, beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
        result_ECF5 = ECF5.result(jet);
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
    injet.setAttribute(m_prefix+"ECF5"+suffix, result_ECF5);

    injet.setAttribute(m_prefix+"ECF1_ungroomed"+suffix, result_ECF1_ungroomed);
    injet.setAttribute(m_prefix+"ECF2_ungroomed"+suffix, result_ECF2_ungroomed);
    injet.setAttribute(m_prefix+"ECF3_ungroomed"+suffix, result_ECF3_ungroomed);

  }

  return 0;
}

