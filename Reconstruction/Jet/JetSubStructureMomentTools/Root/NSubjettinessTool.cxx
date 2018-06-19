/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <mutex>
#include <math.h>
#include <float.h>
#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#include "JetEDM/JetConstituentFiller.h"
#include "JetSubStructureUtils/Nsubjettiness.h"
#include "fastjet/ClusterSequence.hh"
#include "CxxUtils/ubsan_suppress.h"

using namespace std;
using fastjet::PseudoJet;

NSubjettinessTool::NSubjettinessTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Alpha", m_Alpha = 1.0);
}

int NSubjettinessTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  // Supress a warning about undefined behavior in the fastjet
  // WTA_KT_Axes ctor:
  // .../fastjet/contrib/AxesDefinition.hh:551:43: runtime error: member access within address 0x7ffd770850d0 which does not point to an object of type 'WTA_KT_Axes'
  // 0x7ffd770850d0: note: object has invalid vptr
  std::once_flag oflag;
  std::call_once (oflag, CxxUtils::ubsan_suppress,
                  []() { fastjet::contrib::WTA_KT_Axes x; });

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
