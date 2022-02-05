// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_JETTAG_DECORATOR_H
#define I_JETTAG_DECORATOR_H

#include "IDependencyReporter.h"

#include "AsgTools/IAsgTool.h"
#include "xAODJet/JetFwd.h"

class IJetTagDecorator : virtual public asg::IAsgTool,
                         virtual public IDependencyReporter
{
ASG_TOOL_INTERFACE(IJetTagDecorator)

public:

  /// Destructor.
  virtual ~IJetTagDecorator() { };

  /// Method to decorate a jet.
  virtual void decorate(const xAOD::Jet& jet) const = 0;

};


#endif
