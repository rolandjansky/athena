/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//JetConstituentModifierBase.h

#ifndef JETRECTOOLS_JETCONSTITUENTMODIFIERBASE_H
#define JETRECTOOLS_JETCONSTITUENTMODIFIERBASE_H

//// Michael Nelson, CERN & Univesity of Oxford
//// February, 2016

#include "AsgTools/AsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "JetInterface/IJetConstituentModifier.h"

// JetConstituentModifierBase provides default execute () behaviour
// to all JetConstituentModifier tools. Can be used in Rootcore, without
// the need for a sequence.

class JetConstituentModifierBase: public asg::AsgTool, virtual public IJetConstituentModifier {
ASG_TOOL_CLASS(JetConstituentModifierBase, IJetConstituentModifier) 
public:
  JetConstituentModifierBase(const std::string & name);
  int execute() const;
  StatusCode process(xAOD::IParticleContainer* cont) const;
};

#endif

