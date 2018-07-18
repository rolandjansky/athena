/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source file for the JetConstituentModSequence.h
// Michael Nelson, CERN & University of Oxford 
// Will later add the intermediate step

#include "JetRecTools/JetConstituentModSequence.h"
#include "AthContainers/ConstDataVector.h"

JetConstituentModSequence::JetConstituentModSequence(const std::string &name):
  asg::AsgTool(name),
  m_trigInputConstits(NULL), m_trigOutputConstits(NULL) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("InputContainer", m_inputContainer, "The input container for the sequence.");
  declareProperty("OutputContainer", m_outputContainer, "The output container for the sequence.");
  declareProperty("InputType", m_inputType, "The xAOD type name for the input container.");
  declareProperty("Modifiers", m_modifiers, "List of IJet tools.");
	declareProperty("Trigger", m_trigger=false);
  declareProperty("SaveAsShallow", m_saveAsShallow=true, "Save as shallow copy");

}

StatusCode JetConstituentModSequence::initialize() {
  ATH_MSG_INFO("Initializing tool " << name() << "...");
  ATH_MSG_DEBUG("initializing version with data handles");


  ATH_CHECK( m_modifiers.retrieve() );
  if( m_modifiers.empty() ) {
    ATH_MSG_ERROR(" empty container !!" );
    return StatusCode::FAILURE;
  }

  // Set and initialise DataHandleKeys only for the correct input type
  // Die if the input type is unsupported
  switch(m_inputType) {
  case xAOD::Type::CaloCluster:
    {
      m_inClusterKey = m_inputContainer;
      m_outClusterKey = m_outputContainer;
  
      ATH_CHECK(m_inClusterKey.initialize());
      ATH_CHECK(m_outClusterKey.initialize());
      break;
    }
  case xAOD::Type::ParticleFlow:
    {
      m_inChargedPFOKey = m_inputContainer + "ChargedParticleFlowObjects";
      m_inNeutralPFOKey = m_inputContainer + "NeutralParticleFlowObjects";

      m_outChargedPFOKey = m_outputContainer+"ChargedParticleFlowObjects";
      m_outNeutralPFOKey = m_outputContainer+"NeutralParticleFlowObjects";
      m_outAllPFOKey = m_outputContainer+"ParticleFlowObjects";

      ATH_CHECK(m_inChargedPFOKey.initialize());
      ATH_CHECK(m_inNeutralPFOKey.initialize());
      ATH_CHECK(m_outChargedPFOKey.initialize());
      ATH_CHECK(m_outNeutralPFOKey.initialize());
      ATH_CHECK(m_outAllPFOKey.initialize());
      break;
    }
  default:
    ATH_MSG_ERROR(" Unsupported input type "<< m_inputType );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
  
int JetConstituentModSequence::execute() const {
  if (m_trigger){return 0;}

  // Create the shallow copy according to the input type
  switch(m_inputType){

  case xAOD::Type::CaloCluster: {
    auto sc  = copyModRecord(m_inClusterKey, 
                             m_outClusterKey);
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
  
  return 0;
}

StatusCode
JetConstituentModSequence::copyModRecordPFO() const {

  // write in charged and neutral PFO object,
  // pass them to copyModRecord so that copies are
  // written to store gate. Request that the  copies not be modified.

  using PFOCont = xAOD::PFOContainer;
  ATH_CHECK(copyModRecord<PFOCont>(m_inChargedPFOKey, 
                                   m_outChargedPFOKey));

  ATH_CHECK(copyModRecord<PFOCont>(m_inNeutralPFOKey, 
                                   m_outNeutralPFOKey));


  /* read in copies of neutral and chargeed PFO objects just written out,
     place into a single data vector, modify them, and write them out */

  auto outHandle = makeHandle(m_outAllPFOKey);

  ATH_CHECK(outHandle.record(std::make_unique<ConstDataVector<xAOD::PFOContainer>>(SG::VIEW_ELEMENTS)));
  
                             
  auto neutralHandle = makeHandle(m_inNeutralPFOCopyKey);
  if(!neutralHandle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve copy of neutral PFOs from " 
                    << m_inNeutralPFOCopyKey.key());
    return StatusCode::FAILURE;
  }


  auto chargedHandle = makeHandle(m_inChargedPFOCopyKey);
  if(!chargedHandle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve copy of charged PFOs from " 
                    << m_inChargedPFOCopyKey.key());
    return StatusCode::FAILURE;
  }

  outHandle->assign(neutralHandle->begin(), neutralHandle->end());
  outHandle->insert(outHandle->end(),
                    chargedHandle->begin(), 
                    chargedHandle->end());
  
  return StatusCode::SUCCESS;
}

void JetConstituentModSequence::setInputClusterCollection(const xAOD::IParticleContainer *cont) {
  m_trigInputConstits = cont;
}

xAOD::IParticleContainer* JetConstituentModSequence::getOutputClusterCollection() {
  return m_trigOutputConstits;
}
