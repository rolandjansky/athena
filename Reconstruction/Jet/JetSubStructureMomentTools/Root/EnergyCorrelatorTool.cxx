/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorTool.h"
#include "JetSubStructureUtils/EnergyCorrelator.h" 

using namespace std;
using fastjet::PseudoJet;

EnergyCorrelatorTool::EnergyCorrelatorTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing EnergyCorrelator tool.");
  declareProperty("Beta", m_Beta = 1.0);
  declareProperty("IncludeBeta2", m_includeBeta2 = false);
  declareProperty("IncludeECF4", m_includeECF4 = false);
  declareProperty("InputContainer", m_inputContainer = "");
  declareProperty("Prefix", m_prefix = "");
}

StatusCode EnergyCorrelatorTool::initialize()
{

  if(!m_inputContainer.empty() && m_prefix.empty()) {
    ATH_MSG_WARNING("No prefix defined, defaulting to " << m_inputContainer);
  }

  if(!m_prefix.empty() && m_prefix.find_last_of("_")+1 != m_prefix.size()) m_prefix += "_";

  ATH_MSG_DEBUG("InputContainer = " << m_inputContainer);
  ATH_MSG_DEBUG("Prefix = " << m_prefix);

  return StatusCode::SUCCESS;
}

int EnergyCorrelatorTool::modifyJet(xAOD::Jet &injet) const {
  
  bool calculateMoments = true;

  PseudoJet jet;
  if(m_inputContainer.empty()) {
    if(!checkForConstituents(injet)) calculateMoments = false;
    else jet = buildPseudoJet(injet);
  }
  else {
    std::vector<const xAOD::IParticle*> AssociatedParticles;
    if (!(injet.getAssociatedObjects(m_inputContainer,AssociatedParticles))){
      ATH_MSG_WARNING("InputContainer " << m_inputContainer << " unknown. Exiting..");
      return 1;
    }
    ATH_MSG_VERBOSE("Found " << AssociatedParticles.size() << " associated particles");
    if(!AssociatedParticles.size()) calculateMoments = false;
    else jet = buildPseudoJet(AssociatedParticles);
  }

  float result_ECF1 = -999.0;
  float result_ECF2 = -999.0;
  float result_ECF3 = -999.0;
  float result_ECF4 = -999.0;

  float result_ECF1_beta2 = -999.0;
  float result_ECF2_beta2 = -999.0;
  float result_ECF3_beta2 = -999.0;
  float result_ECF4_beta2 = -999.0;

  JetSubStructureUtils::EnergyCorrelator ECF1(1, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelator ECF2(2, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelator ECF3(3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);

  if(calculateMoments) {
    result_ECF1 = ECF1.result(jet);
    result_ECF2 = ECF2.result(jet);
    result_ECF3 = ECF3.result(jet);
  }

  injet.setAttribute(m_prefix+"ECF1", result_ECF1);
  injet.setAttribute(m_prefix+"ECF2", result_ECF2);
  injet.setAttribute(m_prefix+"ECF3", result_ECF3);
  if(m_includeECF4) {
    JetSubStructureUtils::EnergyCorrelator ECF4(4, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
    if(calculateMoments) result_ECF4 = ECF4.result(jet);
    injet.setAttribute(m_prefix+"ECF4", result_ECF4);
  }

  if(m_includeBeta2) {
    JetSubStructureUtils::EnergyCorrelator ECF1_beta2(1, 2.0, JetSubStructureUtils::EnergyCorrelator::pt_R);
    JetSubStructureUtils::EnergyCorrelator ECF2_beta2(2, 2.0, JetSubStructureUtils::EnergyCorrelator::pt_R);
    JetSubStructureUtils::EnergyCorrelator ECF3_beta2(3, 2.0, JetSubStructureUtils::EnergyCorrelator::pt_R);

    if(calculateMoments) {
      result_ECF1_beta2 = ECF1_beta2.result(jet);
      result_ECF2_beta2 = ECF2_beta2.result(jet);
      result_ECF3_beta2 = ECF3_beta2.result(jet);
    }

    injet.setAttribute(m_prefix+"ECF1_Beta2", result_ECF1_beta2);
    injet.setAttribute(m_prefix+"ECF2_Beta2", result_ECF2_beta2);
    injet.setAttribute(m_prefix+"ECF3_Beta2", result_ECF3_beta2);
    if(m_includeECF4) {
      JetSubStructureUtils::EnergyCorrelator ECF4_beta2(4, 2.0, JetSubStructureUtils::EnergyCorrelator::pt_R);
      if(calculateMoments) result_ECF4_beta2 = ECF4_beta2.result(jet);
      injet.setAttribute(m_prefix+"ECF4_Beta2", result_ECF4_beta2);
    }
  }

  return 0;
}
