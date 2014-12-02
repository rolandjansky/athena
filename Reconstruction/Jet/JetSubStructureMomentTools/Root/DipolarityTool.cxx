/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureUtils/Dipolarity.h"

using namespace std;

DipolarityTool::DipolarityTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("SubJetRadius", m_SubJetRadius = 0.3);
}

int DipolarityTool::modifyJet(xAOD::Jet &jet) const
{
  if(checkForConstituents(jet) == false) return 1;

  JetSubStructureUtils::Dipolarity dip12(1, 2, false, m_SubJetRadius);
  JetSubStructureUtils::Dipolarity dip13(1, 3, false, m_SubJetRadius);
  JetSubStructureUtils::Dipolarity dip23(2, 3, false, m_SubJetRadius);
  JetSubStructureUtils::Dipolarity dipexcl12(1, 2, true, m_SubJetRadius);

  jet.setAttribute("Dip12", dip12.result(jet));
  jet.setAttribute("Dip13", dip13.result(jet));
  jet.setAttribute("Dip23", dip23.result(jet));
  jet.setAttribute("DipExcl12", dipexcl12.result(jet));

  return 0;
}
