/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetGroomer.h

#ifndef IJetGroomer_H
#define IJetGroomer_H

// David Adams
// January 2014

/// IJetGroomer is a dual-use tool interface for grooming jets,
/// i.e.\ transforming one jet into another.

#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "JetInterface/IJetPseudojetRetriever.h"

class IJetGroomer : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetGroomer)

public:

  /// Ctor.
  IJetGroomer();

  /// Destructor.
  virtual ~IJetGroomer();

  /// Transform jet. If accepted, the new jet is added to the container.
  /// Return codes:
  ///  -  0 for success.
  ///  - >0 if jet is rejected
  ///  - <0 for error
  virtual int groom(const xAOD::Jet& jin, xAOD::JetContainer& jout) const =0;

  /// Return the pseudojet retriever associated with this tool.
  virtual const IJetPseudojetRetriever* pseudojetRetriever() const;

  /// Set the pseudojet retriever associated with this tool.
  virtual void setPseudojetRetriever(const IJetPseudojetRetriever* ppjr);

private:

  /// Pseudojet retriever.
  const IJetPseudojetRetriever* m_ppjr;

};

#endif
