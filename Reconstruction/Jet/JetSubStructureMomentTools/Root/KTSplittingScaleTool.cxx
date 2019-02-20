/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureUtils/KtSplittingScale.h"
#include "JetSubStructureUtils/ZCut.h"

using fastjet::PseudoJet;

KTSplittingScaleTool::KTSplittingScaleTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int KTSplittingScaleTool::modifyJet(xAOD::Jet &injet) const
{
  
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  float Split12_value = -999, Split23_value = -999, Split34_value = -999,
    ZCut12_value = -999, ZCut23_value = -999, ZCut34_value = -999;

  if (decorate) {
    JetSubStructureUtils::KtSplittingScale split12(1);
    JetSubStructureUtils::KtSplittingScale split23(2);
    JetSubStructureUtils::KtSplittingScale split34(3);
    Split12_value = split12.result(jet);
    Split23_value = split23.result(jet);
    Split34_value = split34.result(jet);
    
    JetSubStructureUtils::ZCut zcut12(1);
    JetSubStructureUtils::ZCut zcut23(2);
    JetSubStructureUtils::ZCut zcut34(3);

    ZCut12_value = zcut12.result(jet);
    ZCut23_value = zcut23.result(jet);
    ZCut34_value = zcut34.result(jet);
    
  }

  
  injet.setAttribute(m_prefix+"Split12", Split12_value);
  injet.setAttribute(m_prefix+"Split23", Split23_value);
  injet.setAttribute(m_prefix+"Split34", Split34_value);

  
  injet.setAttribute(m_prefix+"ZCut12", ZCut12_value);
  injet.setAttribute(m_prefix+"ZCut23", ZCut23_value);
  injet.setAttribute(m_prefix+"ZCut34", ZCut34_value);

  return 0;
}
