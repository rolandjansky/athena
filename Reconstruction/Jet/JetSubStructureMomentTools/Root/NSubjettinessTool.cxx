/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <math.h>
#include <float.h>
#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#include "JetEDM/JetConstituentFiller.h"
#include "JetSubStructureUtils/Nsubjettiness.h"
#include "fastjet/ClusterSequence.hh"

using namespace std;
using fastjet::PseudoJet;

NSubjettinessTool::NSubjettinessTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Alpha", m_Alpha = 1.0);
}

int NSubjettinessTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  fastjet::contrib::NormalizedCutoffMeasure normalized_measure(m_Alpha, jet.getSizeParameter(), 1000000);

  fastjet::contrib::KT_Axes kt_axes;
  JetSubStructureUtils::Nsubjettiness tau1(1, kt_axes, normalized_measure);
  JetSubStructureUtils::Nsubjettiness tau2(2, kt_axes, normalized_measure);
  JetSubStructureUtils::Nsubjettiness tau3(3, kt_axes, normalized_measure);
  jet.setAttribute("Tau1", tau1.result(jet));
  jet.setAttribute("Tau2", tau2.result(jet));
  jet.setAttribute("Tau3", tau3.result(jet));

  fastjet::contrib::WTA_KT_Axes wta_kt_axes;
  JetSubStructureUtils::Nsubjettiness tau1_wta(1, wta_kt_axes, normalized_measure);
  JetSubStructureUtils::Nsubjettiness tau2_wta(2, wta_kt_axes, normalized_measure);
  JetSubStructureUtils::Nsubjettiness tau3_wta(3, wta_kt_axes, normalized_measure);
  jet.setAttribute("Tau1_wta", tau1_wta.result(jet));
  jet.setAttribute("Tau2_wta", tau2_wta.result(jet));
  jet.setAttribute("Tau3_wta", tau3_wta.result(jet));

  return 0;
}
