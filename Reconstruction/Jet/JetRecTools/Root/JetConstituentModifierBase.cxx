/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Source file for the JetConstituentModifierBase.h
// // Michael Nelson, CERN & University of Oxford 

#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"

JetConstituentModifierBase::JetConstituentModifierBase(const std::string & name): asg::AsgTool(name) {
#ifdef ASGTOOL_ATHENA  
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("InputType", m_inputType);
}

StatusCode JetConstituentModifierBase::process(xAOD::IParticleContainer* cont) const
{
  // Test that we are operating on the type of object that
  // we will be writing out.
  // By implication, any supporting containers should not
  // be the ones passed to this method...
  if(!cont->empty() && cont->front()->type() != m_inputType) {
    ATH_MSG_ERROR("Object type mismatch! This tool expects " << m_inputType
		  << ", but received " << cont->front()->type());
    return StatusCode::FAILURE;
  }

  ATH_CHECK(process_impl(cont));

  return StatusCode::SUCCESS;
}


  }
}

StatusCode JetConstituentModifierBase::process(xAOD::IParticleContainer*) const { 
  return StatusCode::SUCCESS;
}


int JetConstituentModifierBase::execute() const {

  ATH_MSG_INFO("Executing JetConstituentModifierBase");
  return 0;

}
