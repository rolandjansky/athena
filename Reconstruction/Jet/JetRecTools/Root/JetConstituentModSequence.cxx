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

  m_outPFOAllKey = m_outputContainer+"ParticleFlowObjects";
  m_caloClusterKey = m_outputContainer;
  m_truthParticleKey = m_outputContainer;
  m_trackParticleKey = m_outputContainer;

  m_inCaloClusterKey = m_inputContainer;
  m_inTrackParticleKey = m_inputContainer;
  m_inTruthParticleKey = m_inputContainer;
  m_inPFOChargedKey = m_inputContainer + "ChargedParticleFlowObjects";
  m_inPFONeutralKey = m_inputContainer + "NeutralParticleFlowObjects";

  // allow reading in of containers previously written out
  m_inPFONeutralCopyKey = m_outPFONeutralKey.key();
  m_inPFOChargedCopyKey = m_outPFOChargedKey.key();

  ATH_CHECK(m_outPFOAllKey.initialize());
  ATH_CHECK(m_caloClusterKey.initialize());
  ATH_CHECK(m_truthParticleKey.initialize());
  ATH_CHECK(m_trackParticleKey.initialize());
  ATH_CHECK(m_outPFOChargedKey.initialize());
  ATH_CHECK(m_outPFONeutralKey.initialize());
  
  ATH_CHECK(m_inCaloClusterKey.initialize());
  ATH_CHECK(m_inTruthParticleKey.initialize());
  ATH_CHECK(m_inTrackParticleKey.initialize());
  ATH_CHECK(m_inPFOChargedKey.initialize());
  ATH_CHECK(m_inPFONeutralKey.initialize());
  ATH_CHECK(m_inPFOChargedCopyKey.initialize());
  ATH_CHECK(m_inPFONeutralCopyKey.initialize());

  switch(m_inputType) {
  case xAOD::Type::CaloCluster:
    break;
  case xAOD::Type::ParticleFlow:
    break;
  default:
    ATH_MSG_ERROR(" Unsupported input type "<< m_inputType );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
  
int JetConstituentModSequence::execute() const {
  if (m_trigger){return 0;}
  xAOD::IParticleContainer* modifiedCont = nullptr;

  // Create the shallow copy according to the input type
  switch(m_inputType){

  case xAOD::Type::CaloCluster: {
    auto sc  = copyModRecord(m_inCaloClusterKey, 
                             m_caloClusterKey);
    if(!sc.isSuccess()){return 1;}
    break; 
  }
  
  case xAOD::Type::TruthParticle: {
    auto sc = copyModRecord(m_inTruthParticleKey, 
                            m_truthParticleKey);
    if(!sc.isSuccess()){return 1;}
    break;
  }
        
  case xAOD::Type::TrackParticle: {
    auto sc = copyModRecord(m_inTrackParticleKey, 
                            m_trackParticleKey);
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
  ATH_CHECK(copyModRecord<PFOCont>(m_inPFOChargedKey, 
                                   m_outPFOChargedKey));

  ATH_CHECK(copyModRecord<PFOCont>(m_inPFONeutralKey, 
                                   m_outPFONeutralKey));


  /* read in copies of neutral and chargeed PFO objects just written out,
     place into a single data vector, modify them, and write them out */

  auto outHandle = makeHandle(m_outPFOAllKey);

  ATH_CHECK(outHandle.record(std::make_unique<ConstDataVector<xAOD::PFOContainer>>(SG::VIEW_ELEMENTS)));
  
                             
  auto neutralHandle = makeHandle(m_inPFONeutralCopyKey);
  if(!neutralHandle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve copy of neutral PFOs from " 
                    << m_inPFONeutralCopyKey.key());
    return StatusCode::FAILURE;
  }


  auto chargedHandle = makeHandle(m_inPFOChargedCopyKey);
  if(!chargedHandle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve copy of charged PFOs from " 
                    << m_inPFOChargedCopyKey.key());
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
