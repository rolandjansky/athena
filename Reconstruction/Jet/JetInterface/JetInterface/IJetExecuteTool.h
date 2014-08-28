/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IJetExecuteTool.h

#ifndef IJetExecuteTool_H
#define IJetExecuteTool_H

// David Adams
// January 2014

/// IJetExecuteTool is a dual-use tool interface for generic tools,
/// i.e.\ those that behave like algorithms.

#include "AsgTools/IAsgTool.h"
#include <vector>

class IJetExecuteTool : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IJetExecuteTool)

public:

  /// Destructor.
  virtual ~IJetExecuteTool() { };

  /// Method to be called for each event.
  /// Returns 0 for success.
  virtual int execute() const =0;

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
