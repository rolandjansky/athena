/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <math.h>
#include <float.h>
#include "JetSubStructureMomentTools/SoftDropObservablesTool.h"
#include "JetEDM/JetConstituentFiller.h"
#include "JetSubStructureUtils/SoftDropObservables.h"
#include "fastjet/ClusterSequence.hh"

using fastjet::PseudoJet;

SoftDropObservablesTool::SoftDropObservablesTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int SoftDropObservablesTool::modifyJet(xAOD::Jet &injet) const {
  PseudoJet jet;

  bool decorate = SetupDecoration(jet,injet);

  // Groomed jet moments
  float zg_value = -999, rg_value = -999;

  if (decorate) {
    JetSubStructureUtils::SoftDropObservables zgGetter("zg");
    JetSubStructureUtils::SoftDropObservables rgGetter("rg");

    zg_value = zgGetter.result(jet);
    rg_value = rgGetter.result(jet);
  }

  // Groomed jet moments
  injet.setAttribute(m_prefix+"zg", zg_value);
  injet.setAttribute(m_prefix+"rg", rg_value);


  return 0;
}
