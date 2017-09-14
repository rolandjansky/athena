/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source file for the JetConstituentModSequence.h
// Michael Nelson, CERN & University of Oxford 
// Will later add the intermediate step

#include "JetRecTools/JetConstituentModSequence.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/CaloCluster.h" 
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTruth/TruthParticle.h" 
#include "xAODTruth/TruthParticleContainer.h" 
#include "xAODTruth/TruthParticleAuxContainer.h" 
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFOAuxContainer.h"

JetConstituentModSequence::JetConstituentModSequence(const std::string &name): asg::AsgTool(name), m_trigInputConstits(NULL), m_trigOutputConstits(NULL) {

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
  ATH_CHECK( m_modifiers.retrieve() );
  if( m_modifiers.empty() ) {
    ATH_MSG_ERROR(" empty container !!" );
    return StatusCode::FAILURE;
  }

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
  const xAOD::IParticleContainer* cont = nullptr;
  if (!m_trigger) {
    if(m_inputType != xAOD::Type::ParticleFlow) {
      ATH_CHECK( evtStore()->retrieve(cont, m_inputContainer) );
    }
  } else {
    cont = m_trigInputConstits;
  }

  xAOD::IParticleContainer* modifiedCont = nullptr;
  xAOD::PFOContainer* chargedCopy(nullptr);
  xAOD::PFOContainer* neutralCopy(nullptr);

  // Create the shallow copy according to the input type
  switch(m_inputType){
  case xAOD::Type::CaloCluster : { 
    modifiedCont = copyAndRecord<xAOD::CaloClusterContainer, xAOD::CaloClusterAuxContainer, xAOD::CaloCluster>(cont, !m_trigger);
    break; }
      
  case xAOD::Type::TruthParticle : {
    modifiedCont = copyAndRecord<xAOD::TruthParticleContainer, xAOD::TruthParticleAuxContainer, xAOD::TruthParticle>(cont, !m_trigger);
    break;}
        
  case xAOD::Type::TrackParticle : {
    modifiedCont = copyAndRecord<xAOD::TrackParticleContainer, xAOD::TrackParticleAuxContainer, xAOD::TrackParticle>(cont, !m_trigger);
    break;}


  case xAOD::Type::ParticleFlow : {
    const xAOD::PFOContainer *charged(nullptr);
    const xAOD::PFOContainer *neutral(nullptr);
    ATH_CHECK( evtStore()->retrieve(charged, m_inputContainer+"ChargedParticleFlowObjects") );
    ATH_CHECK( evtStore()->retrieve(neutral, m_inputContainer+"NeutralParticleFlowObjects") );

    chargedCopy = static_cast<xAOD::PFOContainer *>(copyAndRecord<xAOD::PFOContainer, xAOD::PFOAuxContainer, xAOD::PFO>(charged, !m_trigger, "ChargedParticleFlowObjects"));
    neutralCopy = static_cast<xAOD::PFOContainer *>(copyAndRecord<xAOD::PFOContainer, xAOD::PFOAuxContainer, xAOD::PFO>(neutral, !m_trigger, "NeutralParticleFlowObjects"));

    if(!chargedCopy || !neutralCopy) {
      ATH_MSG_ERROR("Unable to record output collections for " << m_outputContainer+"*ParticleFlowObjects" );
      return 1;
    }

    xAOD::PFOContainer* tmpCont = new xAOD::PFOContainer(SG::VIEW_ELEMENTS);
    for ( xAOD::PFO* pfo: *chargedCopy){
      tmpCont->push_back(pfo);
    }
    for ( xAOD::PFO* pfo: *neutralCopy){
      tmpCont->push_back(pfo);
    }
    modifiedCont=tmpCont;

    if(!m_trigger){
      if( evtStore()->record(tmpCont, m_outputContainer+"ParticleFlowObjects").isFailure() ){
        ATH_MSG_ERROR("Unable to record output collections " << m_outputContainer+"*ParticleFlowObjects" );
        return 1;
      }
    }
    break; }

  default: {
    ATH_MSG_WARNING( "Unsupported input type " << m_inputType );
  }


  }

  if(modifiedCont==0) {
    ATH_MSG_WARNING("Could not create a copy of "<< m_inputContainer);
    return 1;
  }

  // Now pass the input container shallow copy through the modifiers 

  // Loop over the modifier tools:
  for (auto t : m_modifiers) { // Here t is a pointer to an IJetConstituentModifier
    if(t->process(modifiedCont).isFailure()){
      ATH_MSG_WARNING("Failure in modifying constituents " << m_outputContainer );
      return 1;
    }
  }

  if(!m_trigger) {
    ATH_CHECK( evtStore()->setConst(modifiedCont) );
    if(m_inputType == xAOD::Type::ParticleFlow) {
      ATH_CHECK( evtStore()->setConst(chargedCopy) );
      ATH_CHECK( evtStore()->setConst(neutralCopy) );
    }
  }

  //To prevent memory leak when modified PFO are not recorded to event store
  if(m_inputType == xAOD::Type::ParticleFlow && m_trigger) delete modifiedCont;
  
  return 0;
}

void JetConstituentModSequence::setInputClusterCollection(const xAOD::IParticleContainer *cont) {
	m_trigInputConstits = cont;
}

xAOD::IParticleContainer* JetConstituentModSequence::getOutputClusterCollection() {
    return m_trigOutputConstits;
}

