/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source file for the JetConstituentModSequence.h
// Michael Nelson, CERN & University of Oxford 
// Will later add the intermediate step

#include "JetRecTools/JetConstituentModSequence.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

JetConstituentModSequence::JetConstituentModSequence(const std::string &name):
  asg::AsgTool(name),
  m_trigInputConstits(NULL), m_trigOutputConstits(NULL),m_modifiers(this) {

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

  // Create the shallow copy according to the input type
  switch(m_inputType){

  case xAOD::Type::CaloCluster: {
    if (m_trigger){
      auto clustersin = dynamic_cast<const xAOD::CaloClusterContainer*>(m_trigInputConstits);
      if(clustersin==nullptr) {
	ATH_MSG_ERROR("Failed to cast trigInputConstits to CaloCluster");
	return(3);
      }
      auto sc  = copyModForTrigger<xAOD::CaloClusterContainer,xAOD::CaloClusterAuxContainer,xAOD::CaloCluster>(*clustersin);
      if(!sc.isSuccess()){return 1;}
    } else {
      auto sc  = copyModRecord(m_inClusterKey, 
			       m_outClusterKey);
      if(!sc.isSuccess()){return 1;}
    }
    break; 
  }
  
  case xAOD::Type::ParticleFlow: {
    if (m_trigger){return 2;}
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

  // Cannot be handled the same way as other objects (e.g. clusters),
  // because the data is split between two containers, but we need
  // information from both to do the modifications.
  //
  // The logic is:
  //   1. Copy the charged & neutral containers independently
  //   2. Merge into a combined view container
  //   3. Modify the combined container

  // 1. Retrieve the input containers
  auto inNeutralPFOHandle = makeHandle(m_inNeutralPFOKey);
  auto inChargedPFOHandle = makeHandle(m_inChargedPFOKey);
  if(!inNeutralPFOHandle.isValid()){
    ATH_MSG_WARNING("Unable to retrieve input PFO containers \""
		    << m_inNeutralPFOKey.key() << "\" and \""
		    << m_inChargedPFOKey.key() << "\"");
    return StatusCode::FAILURE;
  }

  //    Copy the input containers individually, set I/O option and record
  //    Neutral PFOs
  std::pair<xAOD::PFOContainer*, xAOD::ShallowAuxContainer* > neutralCopy =
    xAOD::shallowCopyContainer(*inNeutralPFOHandle);
  neutralCopy.second->setShallowIO(m_saveAsShallow);
  //
  auto outNeutralPFOHandle = makeHandle(m_outNeutralPFOKey);
  ATH_CHECK(outNeutralPFOHandle.record(std::unique_ptr<xAOD::PFOContainer>(neutralCopy.first),
				       std::unique_ptr<xAOD::ShallowAuxContainer>(neutralCopy.second)));
  //    Charged PFOs
  std::pair<xAOD::PFOContainer*, xAOD::ShallowAuxContainer* > chargedCopy =
    xAOD::shallowCopyContainer(*inChargedPFOHandle);
  chargedCopy.second->setShallowIO(m_saveAsShallow);
  //
  auto outChargedPFOHandle = makeHandle(m_outChargedPFOKey);
  ATH_CHECK(outChargedPFOHandle.record(std::unique_ptr<xAOD::PFOContainer>(chargedCopy.first),
				       std::unique_ptr<xAOD::ShallowAuxContainer>(chargedCopy.second)));

  // 2. Set up output handle for merged (view) container and record
  auto outAllPFOHandle = makeHandle(m_outAllPFOKey);
  ATH_CHECK(outAllPFOHandle.record(std::make_unique<xAOD::PFOContainer>(SG::VIEW_ELEMENTS)));
  //    Merge charged & neutral PFOs into the viw container
  outAllPFOHandle->assign(outNeutralPFOHandle->begin(), outNeutralPFOHandle->end());
  outAllPFOHandle->insert(outAllPFOHandle->end(),
			  outChargedPFOHandle->begin(), 
			  outChargedPFOHandle->end());

  // 3. Now process modifications on all PFOs
  for (auto t : m_modifiers) {ATH_CHECK(t->process(outAllPFOHandle.ptr()));}

  return StatusCode::SUCCESS;
}

void JetConstituentModSequence::setInputClusterCollection(const xAOD::IParticleContainer *cont) {
  m_trigInputConstits = cont;
}

xAOD::IParticleContainer* JetConstituentModSequence::getOutputClusterCollection() {
  return m_trigOutputConstits;
}
