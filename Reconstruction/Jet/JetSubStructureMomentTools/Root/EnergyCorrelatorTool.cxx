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
}

int EnergyCorrelatorTool::modifyJet(xAOD::Jet &jet) const {
  JetSubStructureUtils::EnergyCorrelator ECF1(1, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelator ECF2(2, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  JetSubStructureUtils::EnergyCorrelator ECF3(3, m_Beta, JetSubStructureUtils::EnergyCorrelator::pt_R);
  jet.setAttribute("ECF1", ECF1.result(jet));
  jet.setAttribute("ECF2", ECF2.result(jet));
  jet.setAttribute("ECF3", ECF3.result(jet));
  return 0;
}
