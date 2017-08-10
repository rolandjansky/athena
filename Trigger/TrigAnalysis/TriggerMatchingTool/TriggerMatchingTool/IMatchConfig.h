// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMATCHCONFIG_H
#define IMATCHCONFIG_H

#include <memory>
#include <vector>

#include "xAODBase/ObjectType.h"

namespace Trig {
  
  class IMatchConfig {
  public:
    virtual ~IMatchConfig(){}; //so we can delete properly via base pointer
    virtual double matchThreshold(xAOD::Type::ObjectType) const = 0;
  };
  
}

#endif
