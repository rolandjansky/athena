// This file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JetMomentTools_JETCONSTITREMOVER_H
#define JetMomentTools_JETCONSTITREMOVER_H
///////////////////////////////////////////////////////
/// \class JetConstitRemover
/// \author P-A Delsart
/// \date Dec. 2014
///
/// Tool to remove constituents of a jet.
/// Used to maximaly reduce the truth jet sizes in 
/// pile-up evgen files. 
///////////////////////////////////////////////////////

#include "JetRec/JetModifierBase.h"

class JetConstitRemover : public JetModifierBase {
  ASG_TOOL_CLASS(JetConstitRemover, IJetModifier)
    
public:

  // Constructor from tool name.
  JetConstitRemover(const std::string& myname);

  // Inherited method to modify a jet.
  // 
  virtual int modifyJet(xAOD::Jet& jet) const;

};

#endif
