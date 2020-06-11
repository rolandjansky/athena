/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTrimming.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/Selector.hh"
#include "fastjet/tools/Filter.hh"
#include "JetEDM/PseudoJetVector.h"

#include "JetRec/PseudoJetTranslator.h"


using fastjet::PseudoJet;
using xAOD::JetContainer;



//**********************************************************************

StatusCode JetTrimming::initialize() {
  if ( m_rclus < 0.0 || m_rclus > 10.0 ) {
    ATH_MSG_ERROR("Invalid value for RClus " << m_rclus);
    return StatusCode::FAILURE;
  }
  if ( m_ptfrac < 0.0 || m_ptfrac > 1.0 ) {
    ATH_MSG_ERROR("Invalid value for PtFrac " << m_ptfrac);
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_inputPseudoJets.initialize() );
  ATH_CHECK( m_inputJetContainer.initialize() );

  ATH_MSG_INFO("     Recluster R: " << m_rclus);
  ATH_MSG_INFO("  pT faction min: " << m_ptfrac);
  return StatusCode::SUCCESS;
}


//**********************************************************************
std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > JetTrimming::getJets() const {
  // Return this in case of any problems
  auto nullreturn = std::make_pair(std::unique_ptr<xAOD::JetContainer>(nullptr), std::unique_ptr<SG::IAuxStore>(nullptr));

  // -----------------------
  // retrieve input
  SG::ReadHandle<xAOD::JetContainer> jetContHandle(m_inputJetContainer);
  if(!jetContHandle.isValid()) {
    ATH_MSG_ERROR("No valid JetContainer with key "<< m_inputJetContainer.key());
    return nullreturn;
  }

  SG::ReadHandle<PseudoJetContainer> pjContHandle(m_inputPseudoJets);
  if(!pjContHandle.isValid()) {
    ATH_MSG_ERROR("No valid PseudoJetContainer with key "<< m_inputPseudoJets.key());
    return nullreturn;
  }

  // Build the container to be returned 
  // Avoid memory leaks with unique_ptr
  auto trimmedJets = std::make_unique<xAOD::JetContainer>();
  auto auxCont = std::make_unique<xAOD::JetAuxContainer>();
  trimmedJets->setStore(auxCont.get());


  // The trimmer 
  fastjet::Filter trimmer(fastjet::JetDefinition(fastjet::kt_algorithm, m_rclus),
                          fastjet::SelectorPtFractionMin(m_ptfrac));

  // tool implementing the operations to convert fastjet::PseudoJet -> xAOD::Jet
  PseudoJetTranslator pjTranslator(false, false); // false => do not save jet areas

  // -----------------------
  // Build a new pointer to a PseudoJetVector containing the final trimmed PseudoJets
  // This allows us to own the vector of PseudoJet which we will put in the evt store.
  // Thus the contained PseudoJet will be kept frozen there and we can safely use pointer to them from the xAOD::Jet objects
  auto trimPJVector = std::make_unique<PseudoJetVector>( );
  trimPJVector->resize( jetContHandle->size() );
  // Accessors to retrieve and set the pointers to PseudoJet 
  static SG::AuxElement::ConstAccessor<const fastjet::PseudoJet*> pjConstAcc("PseudoJet");
  static SG::AuxElement::Accessor<const fastjet::PseudoJet*> pjAcc("PseudoJet");


  // loop over input jets
  int count = 0;
  for (const xAOD::Jet* parentJet: *jetContHandle){
    // retrieve the PseudoJet from the parent :
    const PseudoJet * parentPJ = pjConstAcc(*parentJet);
    
    // Trim :
    PseudoJet trimmedPJ = trimmer(*parentPJ) ;
    (*trimPJVector)[count] = trimmedPJ; // save a *copy* of this trimmed PJ
    
    ATH_MSG_VERBOSE("   Input cluster sequence: " << parentPJ->associated_cluster_sequence());
    ATH_MSG_VERBOSE(" Trimmed cluster sequence: " << trimmedPJ.associated_cluster_sequence());

    // build the xAOD::Jet from the PseudoJet, and put it in the container
    xAOD::Jet* jet = pjTranslator.translate(trimmedPJ, *pjContHandle, *trimmedJets, *parentJet);

    // decorate with the pointer to the PJ we keep in the evt store. 
    pjAcc(*jet) = & (*trimPJVector)[count] ; 
    
    int nptrim = trimmedPJ.pieces().size();
    jet->setAttribute<int>(xAOD::JetAttribute::TransformType, xAOD::JetTransform::Trim);
    jet->setAttribute<int>(xAOD::JetAttribute::NTrimSubjets, nptrim);
    // Need to convert from GaudiProperty
    jet->setAttribute(xAOD::JetAttribute::RClus, float(m_rclus));
    jet->setAttribute(xAOD::JetAttribute::PtFrac, float(m_ptfrac));
    ATH_MSG_DEBUG("Properties after trimming:");
    ATH_MSG_DEBUG("   ncon: " << trimmedPJ.constituents().size() << "/"
		  << parentPJ->constituents().size());
    ATH_MSG_DEBUG("   nsub: " << nptrim);
    count++;
  }

  // -------------------------------------
  // record final PseudoJetVector
  SG::WriteHandle<PseudoJetVector> pjVectorHandle(m_finalPseudoJets);
  if(!pjVectorHandle.record(std::move(trimPJVector))){
    ATH_MSG_ERROR("Can't record PseudoJetVector under key "<< m_finalPseudoJets);
    return nullreturn;
  }
  
  // Return the jet container and aux, use move to transfer
  // ownership of pointers to caller
  return std::make_pair(std::move(trimmedJets), std::move(auxCont));

}
