/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/CenterOfMassShapesTool.h"
#include "JetSubStructureUtils/Thrust.h"
#include "JetSubStructureUtils/FoxWolfram.h"
#include "JetSubStructureUtils/SphericityTensor.h"

using namespace std;
using fastjet::PseudoJet;

CenterOfMassShapesTool::CenterOfMassShapesTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  ATH_MSG_DEBUG("Initializing CenterOfMassShapes tool.");
}

int CenterOfMassShapesTool::modifyJet(xAOD::Jet &injet) const {
  
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  map<string, double> res_t, res_fox, res_s;
  
  res_t["ThrustMin"]     = -999;
  res_t["ThrustMaj"]     = -999;
  res_fox["FoxWolfram0"] = -999;
  res_fox["FoxWolfram1"] = -999;
  res_fox["FoxWolfram2"] = -999;
  res_fox["FoxWolfram3"] = -999;
  res_fox["FoxWolfram4"] = -999;
  res_s["Sphericity"]    = -999;
  res_s["Aplanarity"]    = -999;

  if (decorate) {
    JetSubStructureUtils::Thrust t;
    JetSubStructureUtils::FoxWolfram foxwolfram;
    JetSubStructureUtils::SphericityTensor sphericity;
    res_t   = t.result(jet);
    res_fox = foxwolfram.result(jet);
    res_s   = sphericity.result(jet);
    
  }
    
  injet.setAttribute(m_prefix+"ThrustMin", res_t["ThrustMin"]);
  injet.setAttribute(m_prefix+"ThrustMaj", res_t["ThrustMaj"]);

  
  injet.setAttribute(m_prefix+"FoxWolfram0", res_fox["FoxWolfram0"]);
  injet.setAttribute(m_prefix+"FoxWolfram1", res_fox["FoxWolfram1"]);
  injet.setAttribute(m_prefix+"FoxWolfram2", res_fox["FoxWolfram2"]);
  injet.setAttribute(m_prefix+"FoxWolfram3", res_fox["FoxWolfram3"]);
  injet.setAttribute(m_prefix+"FoxWolfram4", res_fox["FoxWolfram4"]);

  
  injet.setAttribute(m_prefix+"Sphericity", res_s["Sphericity"]);
  injet.setAttribute(m_prefix+"Aplanarity", res_s["Aplanarity"]);

  return 0;
}

