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
#include "xAODTruth/TruthParticle.h" 
#include "xAODTruth/TruthParticleContainer.h" 
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/PFOContainer.h"

JetConstituentModSequence::JetConstituentModSequence(const std::string &name): asg::AsgTool(name) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("InputContainer", m_inputContainer, "The input container for the sequence.");
  declareProperty("OutputContainer", m_outputContainer, "The output container for the sequence.");
  declareProperty("InputType", m_inputTypeName, "The xAOD type name for the input container.");
  declareProperty("Modifiers", m_modifiers, "List of IJet tools.");
  declareProperty("SaveAsShallow", m_saveAsShallow=true, "Save as shallow copy");

}

#ifdef ASGTOOL_ATHENA
StatusCode JetConstituentModSequence::initialize() {
  CHECK( m_modifiers.retrieve() );
  if( m_modifiers.empty() ) {
    ATH_MSG_ERROR(" empty container !!" );
    return StatusCode::FAILURE;
  }

  if(m_inputTypeName == "CaloCluster") m_inputType =  xAOD::Type::CaloCluster;
  else if(m_inputTypeName == "TruthParticle") m_inputType =  xAOD::Type::TruthParticle;
  else if(m_inputTypeName == "TrackParticle") m_inputType = xAOD::Type::TrackParticle;
  else if(m_inputTypeName == "ParticleFlow") m_inputType = xAOD::Type::ParticleFlow;
  else {
    ATH_MSG_ERROR(" Unkonwn input type "<< m_inputType );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}
  
int JetConstituentModSequence::execute() const {
  const xAOD::IParticleContainer* cont = 0;
  CHECK( evtStore()->retrieve(cont, m_inputContainer) );

  xAOD::IParticleContainer* modifiedCont = 0;

  // Create the shallow copy according to the input type
  switch(m_inputType){
  case xAOD::Type::CaloCluster : { 
    modifiedCont = copyAndRecord<xAOD::CaloClusterContainer>(cont);
    break; }
      
  case xAOD::Type::TruthParticle : {
    modifiedCont = copyAndRecord<xAOD::TruthParticleContainer>(cont);
    break;}
        
  case xAOD::Type::TrackParticle : {
    modifiedCont = copyAndRecord<xAOD::TrackParticleContainer>(cont);
    break;}


  case xAOD::Type::ParticleFlow : {
    modifiedCont = copyAndRecord<xAOD::PFOContainer>(cont);
    break; }

  default: {
    ATH_MSG_ERROR( "Unsupported input type " << m_inputType );
  }
    

  }

  if(modifiedCont==0) {
    ATH_MSG_ERROR("Could not create a copy of "<< m_inputContainer);
    return -1;
  }

  // Now pass the input container shallow copy through the modifiers 

  // Loop over the modifier tools:
  for (auto t : m_modifiers) { // Here t is a pointer to an IJetConstituentModifier
    ATH_CHECK(t->process(modifiedCont));    
  }
  
  return 0;
}
#endif

