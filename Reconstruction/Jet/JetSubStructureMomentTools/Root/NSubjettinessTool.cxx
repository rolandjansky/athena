/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/NSubjettinessTool.h"
#include "JetSubStructureUtils/Nsubjettiness.h"
#include "CxxUtils/ubsan_suppress.h"

NSubjettinessTool::NSubjettinessTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("Alpha", m_Alpha = 1.0);
  declareProperty("AlphaList", m_rawAlphaVals = {});
  declareProperty("DoDichroic", m_doDichroic = false);
}

StatusCode NSubjettinessTool::initialize() {
  // Add alpha = 1.0 by default
  m_alphaVals.push_back(1.0);

  // Add alpha = m_Alpha by default to keep backwards compatibility
  if( std::abs(m_Alpha-1.0) > 1.0e-5 ) m_alphaVals.push_back(m_Alpha);

  // Clean up input list of alpha values
  for(float alpha : m_rawAlphaVals) {
    
    // Round to the nearest 0.1
    float alphaFix = round( alpha * 10.0 ) / 10.0;
    if(std::abs(alpha-alphaFix) > 1.0e-5) ATH_MSG_DEBUG("alpha = " << alpha << " has been rounded to " << alphaFix);

    // Skip negative values of alpha
    if(alphaFix < 0.0) {
      ATH_MSG_WARNING("alpha must be positive. Skipping alpha = " << alpha);
      continue;
    }

    // Only store value if it is not already in the list
    if( std::find(m_alphaVals.begin(), m_alphaVals.end(), alphaFix) == m_alphaVals.end() ) m_alphaVals.push_back(alphaFix);
  }

  for(float alpha : m_alphaVals) {
    ATH_MSG_DEBUG("Including alpha = " << alpha);
  }

  ATH_CHECK(JetSubStructureMomentToolsBase::initialize());

  return StatusCode::SUCCESS;
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

  // Supress a warning about undefined behavior in the fastjet
  // WTA_KT_Axes ctor:
  // .../fastjet/contrib/AxesDefinition.hh:551:43: runtime error: member access within address 0x7ffd770850d0 which does not point to an object of type 'WTA_KT_Axes'
  // 0x7ffd770850d0: note: object has invalid vptr
  std::once_flag oflag;
  std::call_once (oflag, CxxUtils::ubsan_suppress,
                  []() { fastjet::contrib::WTA_KT_Axes x; });

  for(float alpha : m_alphaVals) {

    if(alpha < 0.0) {
      ATH_MSG_WARNING("Negative alpha values are not supported. Skipping " << alpha);
      continue;
    }

    std::string suffix = GetAlphaSuffix(alpha);

    fastjet::contrib::NormalizedCutoffMeasure normalized_measure(alpha, injet.getSizeParameter(), 1000000);

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
    injet.setAttribute(m_prefix+"Tau1"+suffix, Tau1_value);
    injet.setAttribute(m_prefix+"Tau2"+suffix, Tau2_value);
    injet.setAttribute(m_prefix+"Tau3"+suffix, Tau3_value);
    injet.setAttribute(m_prefix+"Tau4"+suffix, Tau4_value);

    injet.setAttribute(m_prefix+"Tau1_wta"+suffix, Tau1_wta_value);
    injet.setAttribute(m_prefix+"Tau2_wta"+suffix, Tau2_wta_value);
    injet.setAttribute(m_prefix+"Tau3_wta"+suffix, Tau3_wta_value);
    injet.setAttribute(m_prefix+"Tau4_wta"+suffix, Tau4_wta_value);

    // Ungroomed jet moments
    injet.setAttribute(m_prefix+"Tau2_ungroomed"+suffix, Tau2_ungroomed_value);
    injet.setAttribute(m_prefix+"Tau3_ungroomed"+suffix, Tau3_ungroomed_value);
    injet.setAttribute(m_prefix+"Tau4_ungroomed"+suffix, Tau4_ungroomed_value);

    injet.setAttribute(m_prefix+"Tau2_wta_ungroomed"+suffix, Tau2_wta_ungroomed_value);
    injet.setAttribute(m_prefix+"Tau3_wta_ungroomed"+suffix, Tau3_wta_ungroomed_value);
    injet.setAttribute(m_prefix+"Tau4_wta_ungroomed"+suffix, Tau4_wta_ungroomed_value);

  }

  return 0;
}
