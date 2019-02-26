/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/DipolarityTool.h"
#include "JetSubStructureUtils/Dipolarity.h"

DipolarityTool::DipolarityTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("SubJetRadius", m_SubJetRadius = 0.3);
}

int DipolarityTool::modifyJet(xAOD::Jet &injet) const
{
  
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  float Dip12_value = -999, Dip13_value = -999,
    Dip23_value = -999, DipExcl12_value = -999;

  if (decorate) {
  
    JetSubStructureUtils::Dipolarity dip12(1, 2, false, m_SubJetRadius);
    JetSubStructureUtils::Dipolarity dip13(1, 3, false, m_SubJetRadius);
    JetSubStructureUtils::Dipolarity dip23(2, 3, false, m_SubJetRadius);
    JetSubStructureUtils::Dipolarity dipexcl12(1, 2, true, m_SubJetRadius);

    Dip12_value     = dip12.result(jet);
    Dip13_value     = dip13.result(jet);
    Dip23_value     = dip23.result(jet);
    DipExcl12_value = dipexcl12.result(jet);
    
  }

  injet.setAttribute(m_prefix+"Dip12", Dip12_value);
  injet.setAttribute(m_prefix+"Dip13", Dip13_value);
  injet.setAttribute(m_prefix+"Dip23", Dip23_value);
  injet.setAttribute(m_prefix+"DipExcl12", DipExcl12_value);

  return 0;
}
