/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetModiferBase.h

#ifndef JetRec_JetModiferBase_H
#define JetRec_JetModiferBase_H

///////////////////////////////////////////
///
/// \class JetModiferBase
/// A base class for tool acting on a single jet, implementing IJetModifier
/// by looping over the jet container and calling modifyJet(Jet&) for each
/// one. The derived class must implement modifyJet(Jet&).
////////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"

class JetModifierBase : public asg::AsgTool, 
                        virtual public IJetModifier{
  ASG_TOOL_CLASS(JetModifierBase, IJetModifier)

 public:
  
  /// Ctor.
  JetModifierBase(const std::string& myname);

  /// Loop over calls to modifyJet.
  virtual StatusCode modify(xAOD::JetContainer& jets) const;

 protected:

  /// Modify a single jet. This is obsolete and set to be removed.
  virtual int modifyJet(xAOD::Jet& jet) const;

};

#endif
