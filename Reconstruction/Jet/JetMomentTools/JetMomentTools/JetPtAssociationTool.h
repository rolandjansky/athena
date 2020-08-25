/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetDecorator.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODJet/JetContainer.h"

class JetPtAssociationTool : public asg::AsgTool,
                             virtual public IJetDecorator {
  ASG_TOOL_CLASS(JetPtAssociationTool, IJetDecorator)
    
public:

  typedef std::vector<const xAOD::IParticle*> APVector;
  typedef std::vector<unsigned> IndexVector;
  typedef std::vector<float> FloatVector;

public:

  /// Constructor from tool name.
  JetPtAssociationTool(std::string myname);

  /// Initialization.
  virtual StatusCode initialize() override;

  /// Inherited method to modify a jet.
  /// Extract and record the element link and pT fraction
  /// for the jet with the highest such fraction.
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

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
  Gaudi::Property<std::string> m_aname{this, "AssociationName", "", "Key for association vector"};
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "Input jet container"};

  SG::WriteDecorHandleKey<xAOD::JetContainer> m_assocFracKey{this, "AssociationFractionName", "AssociationFraction", "SG key for output AssociationFraction decoration"};
  SG::WriteDecorHandleKey<xAOD::JetContainer> m_assocLinkKey{this, "AssociationLinkName", "AssociationLink", "SG key for output AssociationLink decoration"};
};

#endif
