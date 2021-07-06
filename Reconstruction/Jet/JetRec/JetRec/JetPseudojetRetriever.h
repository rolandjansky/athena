/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// JetPseudojetRetriever.h

#ifndef JetPseudojetRetriever_H
#define JetPseudojetRetriever_H

/// \class JetPseudojetRetriever
/// \author David Adams
/// \date November 2014
///
/// Tool to construct retrieve the pseudojet associated with a jet.
///
/// No properties.

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetPseudojetRetriever.h"

class JetPseudojetRetriever
: public asg::AsgTool,
  virtual public IJetPseudojetRetriever {
ASG_TOOL_CLASS(JetPseudojetRetriever, IJetPseudojetRetriever)

public:

  // Ctor.
  JetPseudojetRetriever(std::string name);

  // Initialization.
  StatusCode initialize();

  /// Retrieve the pseudojet associate with a jet.
  const fastjet::PseudoJet* pseudojet(const xAOD::Jet& jet) const;

  /// Return the label.
  // For now fised. May later make this a property.
  std::string label() const;

  // Dump to log.
  void print() const;

};

#endif
