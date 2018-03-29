/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetConstituentModifier.h

#ifndef JETINTERFACE_IJETCONSTITUENTMODIFIER_H
#define JETINTERFACE_IJETCONSTITUENTMODIFIER_H

// // Michael Nelson, CERN & Univesity of Oxford
// // February, 2016

#include "AsgTools/IAsgTool.h"
#include "xAODBase/IParticleContainer.h"

class IJetConstituentModifier: virtual public asg::IAsgTool { 
ASG_TOOL_INTERFACE(IJetConstituentModifier)

public:
  virtual StatusCode process (xAOD::IParticleContainer* cont) const = 0;

};

#endif 
