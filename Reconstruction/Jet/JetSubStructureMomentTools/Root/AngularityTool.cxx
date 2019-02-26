/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/AngularityTool.h"
#include "JetSubStructureUtils/Angularity.h"

using fastjet::PseudoJet;

AngularityTool::AngularityTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing angularity tool.");
}

int AngularityTool::modifyJet(xAOD::Jet &injet) const {
  
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  float Angularity_value = -999;
  
  if (decorate) {
    JetSubStructureUtils::Angularity angularity;
    Angularity_value = angularity.result(jet);
    ATH_MSG_VERBOSE("Adding jet angularity: " << Angularity_value);
  }
  
  injet.setAttribute(m_prefix+"Angularity", Angularity_value);
  return 0;
}
