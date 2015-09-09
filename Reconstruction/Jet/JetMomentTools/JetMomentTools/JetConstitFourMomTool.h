/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetConstitFourMomTool.h

#ifndef JetMomentTools_JetConstitFourMomTool_H
#define JetMomentTools_JetConstitFourMomTool_H

/// Russell Smith
/// May 2015
///
/// Tool to attach the LC constituent level 4-vector to EM Jets

#include "JetRec/JetModifierBase.h"
#include <TLorentzVector.h>

class JetConstitFourMomTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetConstitFourMomTool, IJetModifier)

 public:

  // Constructor from tool name.
  JetConstitFourMomTool(std::string myname);

  // Inherited method to modify a jet.
  // calls calcConstitFourVec and saves into the jet
  virtual int modifyJet(xAOD::Jet& jet) const;

  // Local method to calculate and return the four-vector
  void calcConstitFourVec(const xAOD::Jet& jet, xAOD::JetFourMom_t& constitFourVec) const;

 private:
  
  std::string m_jetScaleName;
  int m_constitScale;

};

#endif
