// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_SINGLE_JET_DECORATOR_H
#define I_SINGLE_JET_DECORATOR_H

#include "AsgTools/IAsgTool.h"
#include "xAODJet/Jet.h"

class ISingleJetDecorator : virtual public asg::IAsgTool {
ASG_TOOL_INTERFACE(ISingleJetDecorator)

public:

  /// Destructor.
  virtual ~ISingleJetDecorator() { };

  /// Method to decorate a jet.
  virtual void decorate(const xAOD::Jet& jet) const = 0;

};


#endif
