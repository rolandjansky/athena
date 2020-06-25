/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTrimming.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/Selector.hh"
#include "JetEDM/PseudoJetVector.h"

#include "JetRec/PseudoJetTranslator.h"


  // tool implementing the operations to convert fastjet::PseudoJet -> xAOD::Jet
const static PseudoJetTranslator s_pjTranslator(false, false); // false => do not save jet areas

//**********************************************************************

StatusCode JetTrimming::initialize() {
  ATH_CHECK( JetGroomer::initialize() );

  // Unfortunately not possible to do this because there is no
  // declareProperty overload for CheckedProperty in AsgTool
  //
  // Enforce upper/lower limits of Gaudi::Properties
  // m_rclus.verifier().setBounds(0.,10.);
  // m_ptfrac.verifier().setBounds(0.,1.);
  if ( m_rclus < 0.0 || m_rclus > 10.0 ) {
    ATH_MSG_ERROR("Invalid value " << m_rclus << "for RClus. Allowable range is 0-10");
    return StatusCode::FAILURE;
  }
  if ( m_ptfrac < 0.0 || m_ptfrac > 1.0 ) {
    ATH_MSG_ERROR("Invalid value " << m_ptfrac << "for PtFrac. Allowable range is 0-1");
    return StatusCode::FAILURE;
  }

  // Define the trimmer
  m_trimmer = std::make_unique<fastjet::Filter>(fastjet::JetDefinition(fastjet::kt_algorithm, m_rclus),
						fastjet::SelectorPtFractionMin(m_ptfrac));

  ATH_MSG_INFO("     Recluster R: " << m_rclus);
  ATH_MSG_INFO("  pT faction min: " << m_ptfrac);

  return StatusCode::SUCCESS;
}


//**********************************************************************

void JetTrimming::insertGroomedJet(const xAOD::Jet& parentjet, const PseudoJetContainer& inpjcont, xAOD::JetContainer& outcont, PseudoJetVector& trimpjvec) const {

  const static SG::AuxElement::Accessor<const fastjet::PseudoJet*> s_pjAcc("PseudoJet");
  const static SG::AuxElement::ConstAccessor<const fastjet::PseudoJet*> s_pjConstAcc("PseudoJet");

  // retrieve the PseudoJet from the parent :
  const fastjet::PseudoJet& parentPJ = *s_pjConstAcc(parentjet);

  // Trim :
  fastjet::PseudoJet trimmedPJ = m_trimmer->result(parentPJ) ;
  ATH_MSG_VERBOSE("   Input cluster sequence: " << parentPJ.associated_cluster_sequence());
  ATH_MSG_VERBOSE(" Trimmed cluster sequence: " << trimmedPJ.associated_cluster_sequence());

  // build the xAOD::Jet from the PseudoJet, and put it in the container
  xAOD::Jet& jet = s_pjTranslator.translate(trimmedPJ, inpjcont, outcont, parentjet);
  // The vector is resized externally to match the jet container size,
  // so just fill the corresponding entry
  trimpjvec[jet.index()] = trimmedPJ; // save a *copy* of this trimmed PJ

  // decorate with the pointer to the PJ we keep in the evt store.
  s_pjAcc(jet) = & trimpjvec[jet.index()];

  int nptrim = trimmedPJ.pieces().size();
  jet.setAttribute<int>(xAOD::JetAttribute::TransformType, xAOD::JetTransform::Trim);
  jet.setAttribute<int>(xAOD::JetAttribute::NTrimSubjets, nptrim);
  // Need to convert from GaudiProperty
  jet.setAttribute(xAOD::JetAttribute::RClus, float(m_rclus));
  jet.setAttribute(xAOD::JetAttribute::PtFrac, float(m_ptfrac));
  ATH_MSG_VERBOSE("Properties after trimming:");
  ATH_MSG_VERBOSE("   ncon: " << trimmedPJ.constituents().size() << "/"
		  << parentPJ.constituents().size());
  ATH_MSG_VERBOSE("   nsub: " << nptrim);
}
