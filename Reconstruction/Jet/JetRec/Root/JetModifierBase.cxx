/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetModifierBase.h"

//**********************************************************************

JetModifierBase::JetModifierBase(const std::string& myname)
: asg::AsgTool(myname) {
#ifdef ASGTOOL_ATHENA
  // Should we do this here or let the subclasses add this?
  declareInterface<IJetModifier>(this);
#endif
}

//**********************************************************************

int JetModifierBase::modify(xAOD::JetContainer& jets) const {
  for ( xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ++ijet) {
    modifyJet(**ijet);
  }
  return 0;
}

//**********************************************************************
