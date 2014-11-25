/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetPseudojetRetriever.h

#ifndef IJetPseudojetRetriever_H
#define IJetPseudojetRetriever_H

/// \class IJetPseudojetRetriever
/// \author David Adams
/// \date November 2014
///
/// IJetPseudojetRetriever is a dual-use tool interface for a tool that retrieves
/// the pseudojet (i.e. the clsuter sequence) associated with a jet.


#include <string>
#include <vector>
#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace fastjet {
class PseudoJet;
}

class IJetPseudojetRetriever : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetPseudojetRetriever)

public:

  /// Destructor.
  virtual ~IJetPseudojetRetriever() { };

  /// Retrieve the pseudojet associate with a jet.
  virtual const fastjet::PseudoJet* pseudojet(const xAOD::Jet& jet) const =0;

  /// Return the label for this retriever.
  virtual std::string label() const {return "";}

};

#endif
