/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetModifierBase.h"

//**********************************************************************

JetModifierBase::JetModifierBase(const std::string& myname)
: asg::AsgTool(myname) {
#ifndef XAOD_STANDALONE
  // Should we do this here or let the subclasses add this?
  declareInterface<IJetModifier>(this);
#endif
}

//**********************************************************************

StatusCode JetModifierBase::modify(xAOD::JetContainer& jets) const {
  for ( xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ++ijet) {
    modifyJet(**ijet);
  }
  return StatusCode::SUCCESS;
}

int JetModifierBase::modifyJet(xAOD::Jet&) const{
  ATH_MSG_WARNING("JetModifierBase::modifyJet() is obsolete, does nothing, and should not be used!");
  return 1;
}
//**********************************************************************
