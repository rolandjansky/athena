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

// For testing (comment as appropriate):


StatusCode JetConstituentModifierBase::process(xAOD::IParticleContainer*) const { 
  return StatusCode::SUCCESS;
}


int JetConstituentModifierBase::execute() const {

  ATH_MSG_INFO("Executing JetConstituentModifierBase");
  return 0;

}

