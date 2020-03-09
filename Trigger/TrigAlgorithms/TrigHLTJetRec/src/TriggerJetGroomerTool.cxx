/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigHLTJetRec/TriggerJetGroomerTool.h"
#include "xAODJet/JetTrigAuxContainer.h"

using xAOD::JetContainer;

//**********************************************************************
TriggerJetGroomerTool::TriggerJetGroomerTool(const std::string& name): 
  AsgTool(name), m_groomer("",this),m_modifiers(this),m_jetPseudoJetRetriever("",this) {
  declareProperty("JetGroomer", m_groomer);
  declareProperty("JetModifiers", m_modifiers);
  declareProperty("JetPseudojetRetriever", m_jetPseudoJetRetriever);
}

//**********************************************************************

StatusCode TriggerJetGroomerTool::initialize() {

  ATH_MSG_INFO("Initializing TriggerJetGroomerTool " << name() << ".");

  if (m_groomer.retrieve().isSuccess()) {
    ATH_MSG_INFO("Retrieved " << m_groomer->name());
  } else {
    ATH_MSG_ERROR("Retrieving IJetGroomer failed");
    return StatusCode::FAILURE;
  }

  if (m_jetPseudoJetRetriever.retrieve().isSuccess()) {
    ATH_MSG_INFO("Retrieved " << m_jetPseudoJetRetriever->name());
  } else {
    ATH_MSG_ERROR("Retrieving IJetPseudoJetRetriever failed");
    return StatusCode::FAILURE;
  }

  // groomer does not get is jetPseudoJetRetriever directly for historical 
  // reasons. Note that the JetPseudoJetRetriever pointer is freed from the 
  // ToolHandle
  m_groomer->setPseudojetRetriever(&*m_jetPseudoJetRetriever);

  ATH_MSG_INFO(name() << " has " << m_modifiers.size() << " jet modifiers.");
  bool modsOK{true};
  
  for(const auto& hmod : m_modifiers){
    if (hmod.retrieve().isSuccess()) {
      ATH_MSG_INFO("Retrieved " << hmod->name());
    } else {
      ATH_MSG_ERROR("Unable to retrieve IJetModifier");
      modsOK = false;
    }
  }

  return modsOK ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//**********************************************************************

StatusCode TriggerJetGroomerTool::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

//**********************************************************************

JetContainer*
TriggerJetGroomerTool::groom(const JetContainer* in,
                             const PseudoJetContainer& pjContainer
                             ) const {
  ATH_MSG_DEBUG("groom() starts");

  // create a container pointer to be managed by yhe evnt store.
  auto out(std::make_unique<JetContainer>());
  out->setStore(new xAOD::JetTrigAuxContainer);

  // return a nullptr on grooming error, groomed jet container otherwise.
  ATH_MSG_DEBUG("grooming starts");
  for(const auto j : *in) {
    if (m_groomer->groom(*j, pjContainer, *out) < 0) {
      ATH_MSG_DEBUG("Grooming failed for jet with energy " 
                    << j->e() 
                    << ", skipping");
    }
  }
  ATH_MSG_DEBUG("grooming ends");
  
  if(m_modifiers.empty()){ return out.release();}

  ATH_MSG_DEBUG("modifier execution starts");
  ATH_MSG_DEBUG("Executing " << m_modifiers.size() << " jet modifiers.");
  for (const auto& mod : m_modifiers){
    ATH_MSG_DEBUG("  Executing modifier " << mod->name());
    if(mod->modify(*out).isFailure())
      ATH_MSG_DEBUG("    Modifier returned FAILURE!");
  }
  ATH_MSG_DEBUG("mofidier execution ends");


  
  return out.release();
}

