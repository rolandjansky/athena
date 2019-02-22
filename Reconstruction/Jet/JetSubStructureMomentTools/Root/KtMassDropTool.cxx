/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/KtMassDropTool.h"
#include "JetSubStructureUtils/KtMassDrop.h"

KtMassDropTool::KtMassDropTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing KtMassDrop tool.");
}

int KtMassDropTool::modifyJet(xAOD::Jet &injet) const {
    
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  double KtMD_value = -999;

  if (decorate) {
    JetSubStructureUtils::KtMassDrop ktmassdrop;
    KtMD_value = ktmassdrop.result(jet);
  }

  ATH_MSG_VERBOSE("Adding jet ktMassDrop: " << KtMD_value);
  injet.setAttribute(m_prefix+"Mu12", KtMD_value);
  return 0;
}
