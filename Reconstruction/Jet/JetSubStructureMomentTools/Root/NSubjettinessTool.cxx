/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#include "JetSubStructureUtils/Nsubjettiness.h"
#include "CxxUtils/ubsan_suppress.h"

using fastjet::PseudoJet;

NSubjettinessTool::NSubjettinessTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Alpha", m_Alpha = 1.0);
}

int NSubjettinessTool::modifyJet(xAOD::Jet &injet) const {
  
  PseudoJet jet;
  bool decorate = SetupDecoration(jet,injet);

  // Supress a warning about undefined behavior in the fastjet
  // WTA_KT_Axes ctor:
  // .../fastjet/contrib/AxesDefinition.hh:551:43: runtime error: member access within address 0x7ffd770850d0 which does not point to an object of type 'WTA_KT_Axes'
  // 0x7ffd770850d0: note: object has invalid vptr
  std::once_flag oflag;
  std::call_once (oflag, CxxUtils::ubsan_suppress,
                  []() { fastjet::contrib::WTA_KT_Axes x; });

  fastjet::contrib::NormalizedCutoffMeasure normalized_measure(m_Alpha, injet.getSizeParameter(), 1000000);

  float Tau1_value = -999, Tau2_value = -999, Tau3_value = -999, Tau4_value = -999,
    Tau1_wta_value = -999, Tau2_wta_value = -999, Tau3_wta_value = -999, Tau4_wta_value = -999;
 
  if (decorate) {
  
    fastjet::contrib::KT_Axes kt_axes;
    JetSubStructureUtils::Nsubjettiness tau1(1, kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau2(2, kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau3(3, kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau4(4, kt_axes, normalized_measure);
    
    Tau1_value = tau1.result(jet);
    Tau2_value = tau2.result(jet);
    Tau3_value = tau3.result(jet);
    Tau4_value = tau4.result(jet);
    
    fastjet::contrib::WTA_KT_Axes wta_kt_axes;
    JetSubStructureUtils::Nsubjettiness tau1_wta(1, wta_kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau2_wta(2, wta_kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau3_wta(3, wta_kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau4_wta(4, wta_kt_axes, normalized_measure);

    Tau1_wta_value = tau1_wta.result(jet);
    Tau2_wta_value = tau2_wta.result(jet);
    Tau3_wta_value = tau3_wta.result(jet);
    Tau4_wta_value = tau4_wta.result(jet);

  }
  
  injet.setAttribute(m_prefix+"Tau1", Tau1_value);
  injet.setAttribute(m_prefix+"Tau2", Tau2_value);
  injet.setAttribute(m_prefix+"Tau3", Tau3_value);
  injet.setAttribute(m_prefix+"Tau4", Tau4_value);
  
  injet.setAttribute(m_prefix+"Tau1_wta", Tau1_wta_value);
  injet.setAttribute(m_prefix+"Tau2_wta", Tau2_wta_value);
  injet.setAttribute(m_prefix+"Tau3_wta", Tau3_wta_value);
  injet.setAttribute(m_prefix+"Tau4_wta", Tau4_wta_value);
  
  return 0;
}
