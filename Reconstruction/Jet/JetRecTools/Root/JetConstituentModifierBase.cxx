/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source file for the JetConstituentModifierBase.h
// // Michael Nelson, CERN & University of Oxford 

#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODCore/ShallowCopy.h"

JetConstituentModifierBase::JetConstituentModifierBase(const std::string & name): asg::AsgTool(name) {
#ifdef ASGTOOL_ATHENA  
  declareInterface<IJetConstituentModifier>(this);
#endif
}

//xAOD::IParticleContainer* cont = 0; MEN: moved into the execute() method
/*
StatusCode JetConstituentModifierBase::execute () { // MEN: Make this blank for now, just during the testing. See below.
  xAOD::IParticleContainer* cont = 0;
  if(m_inputContainer) {
    CHECK( evtStore()->retrieve(cont, m_inputContainer) );
    std::pair< xAOD::CaloClusterContainer*, xAOD::ShallowAuxContainer* > newclust = xAOD::shallowCopyContainer( *cont ); // MEN: Should we make the container type more generic here, since we will have to add truth, track, pFlow eventually?
  }

  process(cont);

  if(evtStore()->record( newclust.first, m_outputContainer ).isFailure() || evtStore()->record( newclust.second, m_outputContainer+"Aux." ).isFailure() ){

  ATH_MSG_ERROR("Unable to record cluster collection" << m_inputContainer );

  return StatusCode::FAILURE;

  }
}
*/

// For testing (comment as appropriate):


StatusCode JetConstituentModifierBase::process(xAOD::IParticleContainer*) const { 
  return StatusCode::SUCCESS;
}


int JetConstituentModifierBase::execute() const {

  ATH_MSG_INFO("Executing JetConstituentModifierBase");
  return 0;

}

