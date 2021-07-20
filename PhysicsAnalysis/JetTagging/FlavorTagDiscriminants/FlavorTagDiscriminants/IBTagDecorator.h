// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_BTAG_DECORATOR_H
#define I_BTAG_DECORATOR_H

#include "AsgTools/IAsgTool.h"
#include "xAODBTagging/BTaggingFwd.h"

class IBTagDecorator : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(IBTagDecorator)

public:

  /// Destructor.
  virtual ~IBTagDecorator() { };

  /// Method to decorate a jet.
  virtual void decorate(const xAOD::BTagging& btag) const = 0;

  // Names of the decorations being added
  virtual std::set<std::string> getDecoratorKeys() const = 0;
  virtual std::set<std::string> getAuxInputKeys() const = 0;
  virtual std::set<std::string> getConstituentAuxInputKeys() const = 0;
};


#endif
