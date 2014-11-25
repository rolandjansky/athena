/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetConstituentsRetriever.h

#ifndef IJetConstituentsRetriever_H
#define IJetConstituentsRetriever_H

/// \class IJetConstituentsRetriever
/// \author David Adams
/// \date November 2014
///
/// IJetConstituentsRetriever is a dual-use tool interface for a tool that retrieves
/// the constituents of a jet.


#include <string>
#include <vector>
#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetContainer.h"

namespace fastjet {
class PseudoJet;
}

class IJetConstituentsRetriever : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetConstituentsRetriever)

public:

  typedef std::vector<fastjet::PseudoJet> PseudoJetVector;
  typedef std::vector<std::string> NameList;

  /// Destructor.
  virtual ~IJetConstituentsRetriever() { };

  /// Retrieve the constituents as pseudojets.
  /// Appends constituents to cons. Returns 0 for success.
  /// If pghostlabs is not null, then the list of ghost labels
  /// (Track, Truth, ...) is appended to that vector.
  virtual int constituents(const xAOD::Jet& jet, PseudoJetVector& cons,
                           NameList* pghostlabs =nullptr) const =0;

};

#endif
