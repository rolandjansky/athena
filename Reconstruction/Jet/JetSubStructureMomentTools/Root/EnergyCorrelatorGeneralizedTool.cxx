/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h" 
#include "JetSubStructureUtils/EnergyCorrelator.h" 

using fastjet::PseudoJet;

EnergyCorrelatorGeneralizedTool::EnergyCorrelatorGeneralizedTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing EnergyCorrelatorGeneralized tool");
  declareProperty("Beta", m_Beta = 1.0);  
  declareProperty("DoN3", m_doN3 = false);  
  declareProperty("DoLSeries", m_doLSeries = false);  
}

int EnergyCorrelatorGeneralizedTool::modifyJet(xAOD::Jet &injet) const {
  
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  // These are necessary for N2 and M2
  float ECFG_2_1_value = -999, ECFG_3_2_value = -999;
  
  // These are necessary for N3
  float ECFG_3_1_value = -999, ECFG_4_2_value = -999;

  // These are for t/H discrimination
  float ECFG_3_3_2_value = -999, ECFG_3_2_2_value = -999, ECFG_3_3_1_value = -999,
    ECFG_4_2_2_value = -999, ECFG_4_4_1_value = -999, ECFG_2_1_2_value = -999, ECFG_3_2_1_value =  -999,
    ECFG_3_1_1_value = -999;
  // N.B. ECFG_angles_n_beta !!

  if (decorate) {

    // These are necessary for N2 and M2

    JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2(2, 3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
    JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1(1, 2, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
    ECFG_2_1_value = ECFG_2_1.result(jet);
    ECFG_3_2_value = ECFG_3_2.result(jet);

    // These are necessary for N3

    if(m_doN3) {
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2(2, 4, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1(1, 3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
      ECFG_3_1_value = ECFG_3_1.result(jet);
      ECFG_4_2_value = ECFG_4_2.result(jet);
    }

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

  injet.setAttribute(m_prefix+"ECFG_2_1", ECFG_2_1_value);
  injet.setAttribute(m_prefix+"ECFG_3_2", ECFG_3_2_value);
  injet.setAttribute(m_prefix+"ECFG_3_1", ECFG_3_1_value);
  injet.setAttribute(m_prefix+"ECFG_4_2", ECFG_4_2_value);
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
