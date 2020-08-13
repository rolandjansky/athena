/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetGroomer.h"

#include "AsgDataHandles/ReadHandle.h"
#include "fastjet/PseudoJet.hh"
#include "JetRec/PseudoJetContainer.h"

using xAOD::JetContainer;

StatusCode JetGroomer::initialize() {

  ATH_MSG_DEBUG("Initializing...");

 if(m_inputJetContainer.empty()){
    ATH_MSG_ERROR("Jet grooming requested with no input ungroomed jets");
    return StatusCode::FAILURE;
  } else if(m_inputPseudoJets.empty()){
    ATH_MSG_ERROR("Jet grooming requested with no input pseudojets");
    return StatusCode::FAILURE;
  }
  else{
    if(!m_finalPseudoJets.empty()){
      ATH_MSG_WARNING("A non-empty value was found for the FinalPseudoJets WriteHandleKey -- this will be ignored!");
    }

    ATH_CHECK(m_inputJetContainer.initialize());
    ATH_CHECK(m_inputPseudoJets.initialize());
    m_finalPseudoJets = name() + "FinalPJ";
    ATH_CHECK(m_finalPseudoJets.initialize());
  }

  return StatusCode::SUCCESS;
}


// Common operations for any jet groomer
std::pair<std::unique_ptr<xAOD::JetContainer>,std::unique_ptr<SG::IAuxStore> > JetGroomer::getJets() const {
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
  auto groomedJets = std::make_unique<xAOD::JetContainer>();
  auto auxCont = std::make_unique<xAOD::JetAuxContainer>();
  groomedJets->setStore(auxCont.get());

  // -----------------------
  // Build a new pointer to a PseudoJetVector containing the final groomed PseudoJets
  // This allows us to own the vector of PseudoJet which we will put in the evt store.
  // Thus the contained PseudoJet will be kept frozen there and we can safely use pointer to them from the xAOD::Jet objects
  auto groomPJVector = std::make_unique<PseudoJetVector>( );
  groomPJVector->resize( jetContHandle->size() );

  // loop over input jets
  for (const xAOD::Jet* parentJet: *jetContHandle){
    // Child will create a groomed jet and insert it into the output container
    this->insertGroomedJet(*parentJet, *pjContHandle, *groomedJets, *groomPJVector);
  }

  // -------------------------------------
  // record final PseudoJetVector
  SG::WriteHandle<PseudoJetVector> pjVectorHandle(m_finalPseudoJets);
  if(!pjVectorHandle.record(std::move(groomPJVector))){
    ATH_MSG_ERROR("Can't record PseudoJetVector under key "<< m_finalPseudoJets);
    return nullreturn;
  }

  return std::make_pair(std::move(groomedJets),std::move(auxCont));
}
