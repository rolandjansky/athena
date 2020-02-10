/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/MultiplicitiesTool.h"
#include "JetSubStructureUtils/Multiplicities.h"

MultiplicitiesTool::MultiplicitiesTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int MultiplicitiesTool::modifyJet(xAOD::Jet &injet) const {
  fastjet::PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);
   
  // multiplicities
  int nCA_value = -1;
  int nISD_value = -1;
  int nOSD_value = -1;
  int LHM_value = -1;

  float beta_optimal = -1.0;
  
  float lambdaNP = 5.0;
  float zcut_optimal = lambdaNP / (injet.pt()*1.e-3*injet.getSizeParameter());

  if (decorate) {
    JetSubStructureUtils::Multiplicities nCAGetter (0, 0, 0); // number of entries in primary Lund jet plane
    JetSubStructureUtils::Multiplicities nISDGetter(-1, 0.007, 0); // https://arxiv.org/pdf/1704.06266.pdf eq. 2.7
    JetSubStructureUtils::Multiplicities nOSDGetter(beta_optimal, zcut_optimal, 0); // "optimal" configuration, eq. 3.20 above
    JetSubStructureUtils::Multiplicities LHMGetter (0, 0, 5.0);  // cut on kt_emission > 5 GeV ('Les Houches' Mult.), not necessarily meant to be ==lambdaNP!

    nCA_value = nCAGetter.result(jet);
    nISD_value = nISDGetter.result(jet);
    nOSD_value = nOSDGetter.result(jet);
    LHM_value = LHMGetter.result(jet);
  }

  // jet multiplicity moments
  injet.setAttribute(m_prefix+"nCA", nCA_value);
  injet.setAttribute(m_prefix+"nISD", nISD_value);
  injet.setAttribute(m_prefix+"nOSD", nOSD_value);
  injet.setAttribute(m_prefix+"LHM", LHM_value);

  return 0;
}
