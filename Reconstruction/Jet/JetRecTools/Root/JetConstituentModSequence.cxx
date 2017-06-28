/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source file for the JetConstituentModSequence.h
// Michael Nelson, CERN & University of Oxford 
// Will later add the intermediate step

#include "JetRecTools/JetConstituentModSequence.h"

JetConstituentModSequence::JetConstituentModSequence(const std::string &name): asg::AsgTool(name) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("InputContainer", m_inputContainer, "The input container for the sequence.");
  declareProperty("OutputContainer", m_outputContainer, "The output container for the sequence.");
  declareProperty("InputType", m_inputTypeName, "The xAOD type name for the input container.");
  declareProperty("Modifiers", m_modifiers, "List of IJet tools.");
	declareProperty("Trigger", m_trigger=false);
  declareProperty("SaveAsShallow", m_saveAsShallow=true, "Save as shallow copy");

}

StatusCode JetConstituentModSequence::initialize() {
  ATH_MSG_INFO("Initializing tool " << name() << "...");


  ATH_CHECK( m_modifiers.retrieve() );
  if( m_modifiers.empty() ) {
    ATH_MSG_ERROR(" empty container !!" );
    return StatusCode::FAILURE;
  }

  m_pfoKey = m_outputContainer+"ParticleFlowObjects";
  m_caloClusterKey = m_outputContainer;
  m_truthParticleKey = m_outputContainer;
  m_trackParticleKey = m_outputContainer;

  m_inputKey = m_inputContainer;
  m_inputPFOChargedKey = m_inputContainer + "ChargedParticleFlowObjects";
  m_inputPFONeutralKey = m_inputContainer + "NeutralParticleFlowObjects";

  ATH_CHECK(m_pfoKey.initialize());
  ATH_CHECK(m_caloClusterKey.initialize());
  ATH_CHECK(m_truthParticleKey.initialize());
  ATH_CHECK(m_trackParticleKey.initialize());
  ATH_CHECK(m_pfoChargedKey.initialize());
  ATH_CHECK(m_pfoNeutralKey.initialize());
  
  ATH_CHECK(m_inputKey.initialize());
  ATH_CHECK(m_inputPFOChargedKey.initialize());
  ATH_CHECK(m_inputPFONeutralKey.initialize());

  if(m_inputTypeName == "CaloCluster") {
    m_inputType =  xAOD::Type::CaloCluster;
  } else if(m_inputTypeName == "TruthParticle") {
    m_inputType =  xAOD::Type::TruthParticle;
  } else if(m_inputTypeName == "TrackParticle") {
    m_inputType = xAOD::Type::TrackParticle;
  } else if(m_inputTypeName == "ParticleFlow") {
    m_inputType = xAOD::Type::ParticleFlow;
  } else {
    ATH_MSG_ERROR(" Unkonwn input type "<< m_inputType );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
  
int JetConstituentModSequence::execute() const {
  if (m_trigger){return 0;}

  /*
  const xAOD::IParticleContainer* cont = nullptr;
  if(!(evtStore()->retrieve(cont, m_inputContainer)).isSuccess()){
    ATH_MSG_ERROR(" Unable to retrieve input container "
                    << m_inputTypeName);
    return StatusCode::FAILURE;
  }
  */


  constexpr auto useModifiers = true;

  // Create the shallow copy according to the input type
  switch(m_inputType){

  case xAOD::Type::CaloCluster: {
    auto cont = getInput(m_inputKey);
    if(cont == nullptr){return 1;}
    auto sc  = copyModRecord(cont, useModifiers, m_caloClusterKey);
    if(!sc.isSuccess()){return 1;}
    break; 
  }
  
  case xAOD::Type::TruthParticle: {
    auto cont = getInput(m_inputKey);
    if(cont == nullptr){return 1;}
    auto sc = copyModRecord(cont, useModifiers, m_truthParticleKey);
    if(!sc.isSuccess()){return 1;}
    break;
  }
        
  case xAOD::Type::TrackParticle: {
    auto cont = getInput(m_inputKey);
    if(cont == nullptr){return 1;}
    auto sc = copyModRecord(cont, useModifiers, m_trackParticleKey);
    if(!sc.isSuccess()){return 1;}
    break;
  }

  case xAOD::Type::ParticleFlow: {
    auto sc = copyModRecordPFO();
    if(!sc.isSuccess()){return 1;}
    break;
  }

  default: {
    ATH_MSG_WARNING( "Unsupported input type " << m_inputType );
    return 1;
  }
    
    
  }
  
  /*
  if(modifiedCont == nullptr) {
    ATH_MSG_WARNING("Could not create a copy of "<< m_inputContainer);
    return 1;
  }
  */

  // Now pass the input container shallow copy through the modifiers 
  
  // Loop over the modifier tools:
  /*
    for (auto t : m_modifiers) {
    if(t->process(modifiedCont).isFailure()){
    ATH_MSG_WARNING("Failure in modifying constituents " << m_outputContainer );
    return 1;
    }
    }
  */
  
  //To prevent memory leak when modified PFO are not recorded to event store
  /*
    if(m_inputType == xAOD::Type::ParticleFlow && m_trigger) delete modifiedCont;
  */

  return 0;
}

void JetConstituentModSequence::setInputClusterCollection(const xAOD::IParticleContainer *cont) {
  m_trigInputClusters = cont;
}


//xAOD::IParticleContainer* 
StatusCode
JetConstituentModSequence::copyModRecordPFO() const {

  // const xAOD::IParticleContainer *charged;
  // const xAOD::IParticleContainer *neutral;

  auto charged = getInput(m_inputPFOChargedKey);
  if(charged == nullptr){
    ATH_MSG_WARNING("Error retrieving PFO charged objects");
    return StatusCode::FAILURE;
  }

  auto neutral = getInput(m_inputPFONeutralKey);
  if(neutral == nullptr){
    ATH_MSG_WARNING("Error retrieving PFO neutral objects");
      return StatusCode::FAILURE;
  }

  /*
  std::string key = m_inputContainer+"ChargedParticleFlowObjects";
  ATH_CHECK(evtStore()->retrieve(charged, key));

  key = m_inputContainer+"NeutralParticleFlowObjects";
  ATH_CHECK(evtStore()->retrieve(neutral, key).isSuccess());
  */

  using PFOCont = xAOD::PFOContainer;
  constexpr auto useModifiers = false;
  ATH_CHECK(copyModRecord<PFOCont>(charged, useModifiers, m_pfoChargedKey));
  ATH_CHECK(copyModRecord<PFOCont>(neutral, useModifiers, m_pfoNeutralKey));
  /*
  using PFOCont = xAOD::PFOContainer;
  PFOCont* chargedCopy = 
    dynamic_cast<PFOCont*>(copyModRecord<PFOCont>(charged, 
                                                  useModifiers,
                                                  m_pfoChargedKey));
  
  PFOCont* neutralCopy = 
    dynamic_cast<PFOCont*>(copyModRecord<PFOCont>(neutral, 
                                                  useModifiers,
                                                  m_pfoNeutralKey));

  PFOCont* modifiedCont = new PFOCont(SG::VIEW_ELEMENTS);
  for (xAOD::PFO* pfo: *chargedCopy) {modifiedCont->push_back(pfo);}
  for (xAOD::PFO* pfo: *neutralCopy) {modifiedCont->push_back(pfo);}
  */

  auto handle = makeHandle(m_pfoKey);
  ATH_CHECK(handle.record(std::make_unique< PFOCont>(SG::VIEW_ELEMENTS)));
  for (auto o : *charged) {handle->push_back(dynamic_cast<xAOD::PFO*>(o));}
  for (auto o : *neutral) {handle->push_back(dynamic_cast<xAOD::PFO*>(o));}
  /*
    PFOCont* pfoCont = new PFOCont(SG::VIEW_ELEMENTS);
    for (auto o : *charged) {pfoCont->push_back(dynamic_cast<xAOD::PFO*>(o));}
    for (auto o : *neutral) {pfoCont->push_back(dynamic_cast<xAOD::PFO*>(o));}
    ATH_CHECK(evtStore()->record(pfoCont, 
    m_outputContainer+"ParticleFlowObjects"));
  */

  return StatusCode::SUCCESS;
}


const xAOD::IParticleContainer* 
JetConstituentModSequence::getInput(const SG::ReadHandleKey<xAOD::IParticleContainer>& key) const {
  
  auto handle = SG::makeHandle(key);

  if(!handle.isValid()){
    ATH_MSG_ERROR("Error retrieving input from evtStore: " 
                  << m_inputKey.key());
    return nullptr;
  }

  return handle.cptr();
}
