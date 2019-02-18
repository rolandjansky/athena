/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  // 332
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_3_2(3, 3, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_3_3_2", ECFG_3_3_2.result(jet));
 
  // 461
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_6_1(6, 4, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_4_6_1", ECFG_4_6_1.result(jet));

  // 322
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2_2(2, 3, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_3_2_2", ECFG_3_2_2.result(jet));

  // 331
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_3_1(3, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_3_3_1", ECFG_3_3_1.result(jet));

  // 422
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2_2(2, 4, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_4_2_2", ECFG_4_2_2.result(jet));

  // 441
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_4_1(4, 4, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_4_4_1", ECFG_4_4_1.result(jet));

  // 212
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1_2(1, 2, 2, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_2_1_2", ECFG_2_1_2.result(jet));

  // 321
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2_1(2, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_3_2_1", ECFG_3_2_1.result(jet));

  // 311
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1_1(1, 3, 1, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECFG_3_1_1", ECFG_3_1_1.result(jet));
  
  return 0;
}
