/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetChargeTool.cxx

#include "JetSubStructureMomentTools/JetChargeTool.h"
#include "JetSubStructureUtils/Charge.h"

using namespace std;

JetChargeTool::JetChargeTool(std::string name)
: JetSubStructureMomentToolsBase(name) {
  declareProperty("K", m_k = 1.0);
}

int JetChargeTool::modifyJet(xAOD::Jet &jet) const {
  JetSubStructureUtils::Charge charge(m_k);
  jet.setAttribute("Charge", charge.result(jet));
  return 0;
}
