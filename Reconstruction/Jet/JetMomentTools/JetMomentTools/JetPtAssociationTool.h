/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetPtAssociationTool.h

#ifndef JetMomentTools_JetPtAssociationTool_H
#define JetMomentTools_JetPtAssociationTool_H

/// David Adams \n
/// April 2014
///
/// Tool to associate jets using the fraction of AP (associated particle) pT
/// shared with the constituents of jets in a matching container.
/// Properties:
///   AssociationName - Name of the association
///   InputContainer - Name of the matching jet container

#include "JetRec/JetModifierBase.h"
#include "xAODJet/JetContainer.h"

class JetPtAssociationTool : public JetModifierBase {
  ASG_TOOL_CLASS(JetPtAssociationTool, IJetModifier)
    
public:

  typedef std::vector<const xAOD::IParticle*> APVector;
  typedef std::vector<unsigned> IndexVector;
  typedef std::vector<float> FloatVector;

public:

  /// Constructor from tool name.
  JetPtAssociationTool(std::string myname);

  /// Initialization.
  StatusCode initialize();

  /// Inherited method to modify a jet.
  /// Extract and record the element link and pT fraction
  /// for the jet with the highest such fraction.
  int modifyJet(xAOD::Jet& jet) const;

private:  // data

  /// Return the matched pT sum for each jet in a collection.
  /// The matching is done with match().
  ///   aps - input AP vector
  ///   jet - Jet to be matched
  ///   ptsums - output vector with matched pT fraction for each jet
  int ptfrac(const APVector& aps, const xAOD::JetContainer& jets, FloatVector& ptsums) const;

  /// Return the vector of AP's that appear as
  /// constituents of the given jet.
  ///   aps - input AP vector
  ///   jet - Jet to be matched
  ///   apvs - output vector of matched AP indices
  ///   ptsum - sum of jet constituent pts
  int match(const APVector& aps, const xAOD::Jet& jet, APVector& apvs, double& ptsum_constituents) const;

  /// Properties.
  std::string m_aname;
  SG::ReadHandleKey<xAOD::JetContainer> m_jetContainer_key;
};

#endif
