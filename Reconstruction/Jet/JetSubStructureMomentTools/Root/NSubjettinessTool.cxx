/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#include "JetSubStructureUtils/Nsubjettiness.h"

NSubjettinessTool::NSubjettinessTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Alpha", m_Alpha = 1.0);
  declareProperty("DoDichroic", m_doDichroic = false);
}

int NSubjettinessTool::modifyJet(xAOD::Jet &injet) const {
  
  fastjet::PseudoJet jet;
  fastjet::PseudoJet jet_ungroomed;

  bool decorate = SetupDecoration(jet,injet);
  bool decorate_ungroomed = false;
  if(m_doDichroic) {
    // Get parent jet here and replace injet
    ElementLink<xAOD::JetContainer> parentLink = injet.auxdata<ElementLink<xAOD::JetContainer> >("Parent");
  
    // Return error is parent element link is broken
    if(!parentLink.isValid()) {
      ATH_MSG_ERROR("Parent element link is not valid. Aborting");
      return 1;
    }

    const xAOD::Jet* parentJet = *(parentLink);
    decorate_ungroomed = SetupDecoration(jet_ungroomed,*parentJet);
  }

  fastjet::contrib::NormalizedCutoffMeasure normalized_measure(m_Alpha, injet.getSizeParameter(), 1000000);

  // Groomed jet moments
  float Tau1_value = -999, Tau2_value = -999, Tau3_value = -999, Tau4_value = -999,
        Tau1_wta_value = -999, Tau2_wta_value = -999, Tau3_wta_value = -999, Tau4_wta_value = -999;

  // Ungroomed jet moments
  float Tau2_ungroomed_value = -999, Tau3_ungroomed_value = -999, Tau4_ungroomed_value = -999,
        Tau2_wta_ungroomed_value = -999, Tau3_wta_ungroomed_value = -999, Tau4_wta_ungroomed_value = -999;

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

    if(decorate_ungroomed) { 
      Tau2_ungroomed_value = tau2.result(jet_ungroomed);
      Tau3_ungroomed_value = tau3.result(jet_ungroomed);
      Tau4_ungroomed_value = tau4.result(jet_ungroomed);
    }

    fastjet::contrib::WTA_KT_Axes wta_kt_axes;
    JetSubStructureUtils::Nsubjettiness tau1_wta(1, wta_kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau2_wta(2, wta_kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau3_wta(3, wta_kt_axes, normalized_measure);
    JetSubStructureUtils::Nsubjettiness tau4_wta(4, wta_kt_axes, normalized_measure);

    Tau1_wta_value = tau1_wta.result(jet);
    Tau2_wta_value = tau2_wta.result(jet);
    Tau3_wta_value = tau3_wta.result(jet);
    Tau4_wta_value = tau4_wta.result(jet);

    if(decorate_ungroomed) { 
      Tau2_wta_ungroomed_value = tau2_wta.result(jet_ungroomed);
      Tau3_wta_ungroomed_value = tau3_wta.result(jet_ungroomed);
      Tau4_wta_ungroomed_value = tau4_wta.result(jet_ungroomed);
    }

  }

  // Groomed jet moments
  injet.setAttribute(m_prefix+"Tau1", Tau1_value);
  injet.setAttribute(m_prefix+"Tau2", Tau2_value);
  injet.setAttribute(m_prefix+"Tau3", Tau3_value);
  injet.setAttribute(m_prefix+"Tau4", Tau4_value);

  injet.setAttribute(m_prefix+"Tau1_wta", Tau1_wta_value);
  injet.setAttribute(m_prefix+"Tau2_wta", Tau2_wta_value);
  injet.setAttribute(m_prefix+"Tau3_wta", Tau3_wta_value);
  injet.setAttribute(m_prefix+"Tau4_wta", Tau4_wta_value);

  // Ungroomed jet moments
  injet.setAttribute(m_prefix+"Tau2_ungroomed", Tau2_ungroomed_value);
  injet.setAttribute(m_prefix+"Tau3_ungroomed", Tau3_ungroomed_value);
  injet.setAttribute(m_prefix+"Tau4_ungroomed", Tau4_ungroomed_value);

  injet.setAttribute(m_prefix+"Tau2_wta_ungroomed", Tau2_wta_ungroomed_value);
  injet.setAttribute(m_prefix+"Tau3_wta_ungroomed", Tau3_wta_ungroomed_value);
  injet.setAttribute(m_prefix+"Tau4_wta_ungroomed", Tau4_wta_ungroomed_value);

  return 0;
}
