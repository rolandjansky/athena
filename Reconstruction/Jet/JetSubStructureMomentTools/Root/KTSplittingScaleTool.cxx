/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/KTSplittingScaleTool.h"
#include "JetSubStructureUtils/KtSplittingScale.h"
#include "JetSubStructureUtils/ZCut.h"

using namespace std;
using fastjet::PseudoJet;

KTSplittingScaleTool::KTSplittingScaleTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int KTSplittingScaleTool::modifyJet(xAOD::Jet &jet) const
{
  if(checkForConstituents(jet) == false) return 1;

  JetSubStructureUtils::KtSplittingScale split12(1);
  JetSubStructureUtils::KtSplittingScale split23(2);
  JetSubStructureUtils::KtSplittingScale split34(3);
  jet.setAttribute("Split12", split12.result(jet));
  jet.setAttribute("Split23", split23.result(jet));
  jet.setAttribute("Split34", split34.result(jet));

  JetSubStructureUtils::ZCut zcut12(1);
  JetSubStructureUtils::ZCut zcut23(2);
  JetSubStructureUtils::ZCut zcut34(3);
  jet.setAttribute("ZCut12", zcut12.result(jet));
  jet.setAttribute("ZCut23", zcut23.result(jet));
  jet.setAttribute("ZCut34", zcut34.result(jet));

  return 0;
}
