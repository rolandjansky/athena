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

  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_2(2, 3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_2_1(1, 2, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_4_2(2, 4, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelatorGeneralized ECFG_3_1(1, 3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);

  jet.setAttribute("ECFG_2_1", ECFG_2_1.result(jet));
  jet.setAttribute("ECFG_3_2", ECFG_3_2.result(jet));
  jet.setAttribute("ECFG_3_1", ECFG_3_1.result(jet));
  jet.setAttribute("ECFG_4_2", ECFG_4_2.result(jet));


  return 0;
}
