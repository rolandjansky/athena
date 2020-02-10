/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetChargedParticlesTool.h

#ifndef JetSimTools_JetChargedParticlesTool_H
#define JetSimTools_JetChargedParticlesTool_H

/// Marie-Helene Genest January 2020
///
/// Tool to calculate the number of charged particles in truth jets

#include "JetRec/JetModifierBase.h"
#include "AsgTools/ToolHandle.h"

class JetChargedParticlesTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetChargedParticlesTool, IJetModifier)
    
public:

  // Constructor from tool name.
  JetChargedParticlesTool(std::string myname);

  // Inherited method to modify a jet.
  // Calls ncharged() and puts the result on the jet.
  virtual int modifyJet(xAOD::Jet& jet) const override;

  // Local method to calculate and return the num of charged particles
  //  IMPORTANT : the function performs a cast on TruthParticle. It works only with truth jets.
  int ncharged(const xAOD::Jet& jet) const;

private:

  float m_minPt = 500; /// minimum pT to count particles
  float m_maxEta = 2.5; /// maximum abs(eta) to count particles
};

#endif
