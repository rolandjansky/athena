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
#include "xAODCaloEvent/CaloClusterChangeSignalState.h"

JetConstituentModSequence::JetConstituentModSequence(const std::string &name): asg::AsgTool(name), m_trigInputClusters(NULL), m_trigOutputClusters(NULL) {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("InputContainer", m_inputContainer, "The input container for the sequence.");
  declareProperty("OutputContainer", m_outputContainer, "The output container for the sequence.");
  declareProperty("InputType", m_inputTypeName, "The xAOD type name for the input container.");
  declareProperty("Modifiers", m_modifiers, "List of IJet tools.");
  declareProperty("Trigger", m_trigger=false);
  declareProperty("EMTrigger", m_isEMTrigger=false);
  declareProperty("SaveAsShallow", m_saveAsShallow=true, "Save as shallow copy");

}

StatusCode JetConstituentModSequence::initialize() {
  ATH_CHECK( m_modifiers.retrieve() );
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
  if (!m_trigger)
    ATH_CHECK( evtStore()->retrieve(cont, m_inputContainer) );
  else
    cont = m_trigInputClusters;

  xAOD::IParticleContainer* modifiedCont = 0;

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
    modifiedCont = copyAndRecord<xAOD::PFOContainer, xAOD::PFOAuxContainer, xAOD::PFO>(cont, !m_trigger);
    break; }

  default: {
    ATH_MSG_WARNING( "Unsupported input type " << m_inputType );
  }
    

  }

  if(modifiedCont==0) {
    ATH_MSG_WARNING("Could not create a copy of "<< m_inputContainer);
    return 1;
  }

  // Handle the change of calibration differently in the trigger in R21, as
  // needing to deep copy costs a lot more memory
  // Add clusters to the list to register a change of state
  // They are restored to original state when the list goes out of scope
  CaloClusterChangeSignalStateList signalStateList;
  if(m_trigger && m_isEMTrigger){
    xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (modifiedCont); // Get CaloCluster container
    for(xAOD::CaloCluster* cl : *clust) {
      signalStateList.add(cl, xAOD::CaloCluster::UNCALIBRATED);
    }
  }

  // Now pass the input container shallow copy through the modifiers 

  // Loop over the modifier tools:
  for (auto t : m_modifiers) { // Here t is a pointer to an IJetConstituentModifier
    if(t->process(modifiedCont).isFailure()){
      ATH_MSG_WARNING("Failure in modifying constituents " << m_outputContainer );
      return 1;
    }
  }
  
  return 0;
}

void JetConstituentModSequence::setInputClusterCollection(const xAOD::IParticleContainer *cont) {
	m_trigInputClusters = cont;
}

xAOD::IParticleContainer* JetConstituentModSequence::getOutputClusterCollection() {
    return m_trigOutputClusters;
}

