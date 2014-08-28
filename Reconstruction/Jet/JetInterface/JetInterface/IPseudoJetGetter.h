/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IPseudoJetGetter.h

#ifndef IPseudoJetGetter_H
#define IPseudoJetGetter_H

// David Adams
// January 2014

#include "AsgTools/IAsgTool.h"
#include <vector>

namespace fastjet {
class PseudoJet;
}
/// Vector of fastjet pseudojets.
typedef std::vector<fastjet::PseudoJet> PseudoJetVector;

/// IPseudoJetGetter is a dual-use tool interface for retrieving
/// a vector of fastjet pseudojets.

class IPseudoJetGetter : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IPseudoJetGetter)

public:

  /// Destructor.
  virtual ~IPseudoJetGetter() { };

  /// Method to return a vector of pseudojets.
  virtual const PseudoJetVector* get() const =0;

  /// Label assignd to this collection of pseudojets.
  virtual std::string label() const =0;

  /// Method to return the list of input containers.
  /// The names of required input containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  virtual int inputContainerNames(std::vector<std::string>& connames);

  /// Method to return the list of output containers.
  /// The names of produced output containers are appended to connames.
  /// Returns nonzero for error.
  /// Default returns 0 and adds no names.
  virtual int outputContainerNames(std::vector<std::string>& connames);

};

#endif
