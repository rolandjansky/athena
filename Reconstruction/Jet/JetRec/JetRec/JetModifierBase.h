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
/// by looping over ISingleJetModifier.
/// 
/// This class provides a default implementation for modify(JetContainer&) which simply
/// loops over the container and calls modify(Jet&)
////////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "JetInterface/ISingleJetModifier.h"
#include "JetInterface/IJetModifier.h"

class JetModifierBase : public asg::AsgTool, 
                        virtual public IJetModifier ,
                        virtual public ISingleJetModifier {
  ASG_TOOL_CLASS(JetModifierBase, ISingleJetModifier)

 public:
  
  /// Ctor.
  JetModifierBase(const std::string& myname);

  /// Loop over calls to modifyJet.
  virtual int modify(xAOD::JetContainer& jets) const;

};

#endif
