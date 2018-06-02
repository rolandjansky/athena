/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h" 
#include "fastjet/contrib/EnergyCorrelator.hh" 
#include "JetSubStructureUtils/EnergyCorrelator.h" 

using namespace std;
using fastjet::PseudoJet;

EnergyCorrelatorGeneralizedTool::EnergyCorrelatorGeneralizedTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing EnergyCorrelator tool.");
  declareProperty("Beta", m_Beta = 1.0);
}

int EnergyCorrelatorGeneralizedTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  // N.B. here _angles, _n, _beta !!

  // These ones are necessary for N2, M2, etc.

  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2(2, 3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1(1, 2, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2(2, 4, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1(1, 3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);

  jet.setAttribute("ECFG_2_1", ECFG_2_1.result(jet));
  jet.setAttribute("ECFG_3_2", ECFG_3_2.result(jet));
  jet.setAttribute("ECFG_3_1", ECFG_3_1.result(jet));
  jet.setAttribute("ECFG_4_2", ECFG_4_2.result(jet));

  // these ones for t/H discrimination

  // 232
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_3_2(2, 3, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_2_3_2", ECFG_2_3_2.result(jet));
 
  // 541
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_5_4_1(5, 4, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_5_4_1", ECFG_5_4_1.result(jet));

  // 132
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_1_3_2(1, 3, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_1_3_2", ECFG_1_3_2.result(jet));

  // 231
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_3_1(2, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_2_3_1", ECFG_2_3_1.result(jet));

  // 142
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_1_4_2(1, 4, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_1_4_2", ECFG_1_4_2.result(jet));

  // 341
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_4_1(3, 4, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_3_4_1", ECFG_3_4_1.result(jet));

  // 022
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_0_2_2(0, 2, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_0_2_2", ECFG_0_2_2.result(jet));

  // 131
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_1_3_1(1, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_1_3_1", ECFG_1_3_1.result(jet));

  // 031
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_0_3_1(0, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_0_3_1", ECFG_0_3_1.result(jet));

  return 0;
}
