// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_DEPENDENCY_REPORTER
#define I_DEPENDENCY_REPORTER

#include <set>
#include <string>

class IDependencyReporter {
public:
  /// Destructor.
  virtual ~IDependencyReporter() { };

  // Names of the decorations being added
  virtual std::set<std::string> getDecoratorKeys() const = 0;
  virtual std::set<std::string> getAuxInputKeys() const = 0;
  virtual std::set<std::string> getConstituentAuxInputKeys() const = 0;
};

#endif
