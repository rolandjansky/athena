/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetECPSFractionTool.h

#ifndef JetMomentTools_JetECPSFractionTool_H
#define JetMomentTools_JetECPSFractionTool_H

/// David Adams \n
/// May 2015
///
/// Tool to calculate the fraction of jet energy in endcap preshower (ECPS) clusters.
/// Clusters which have more that ECPSFractionThreshold of their energy in the ECPS are
/// included in the numerator.
///
/// Properties:
///   ECPSFractionThreshold: Threshold for identifying a cluster as ECPS.
///
/// Attributes:
///   ECPSFraction: Fraction of jet energy in the ECPS
///

#include "JetRec/JetModifierBase.h"

class JetECPSFractionTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetECPSFractionTool, IJetModifier)
    
public:

  // Constructor from tool name.
  JetECPSFractionTool(std::string myname);

  // Inherited method to modify a jet.
  // Calls width and puts the result on the jet.
  int modifyJet(xAOD::Jet& jet) const;

  // Local method to calculate and return the energy fraction.
  double energyFraction(const xAOD::Jet& jet) const;

private:

  // Properties.
  double m_fraclimit;

};

#endif
