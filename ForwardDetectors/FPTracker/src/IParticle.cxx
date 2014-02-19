/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/IParticle.h"

namespace FPTracker
{
  IParticle::~IParticle(){}
  std::ostream& operator<<(std::ostream& os, const IParticle& p)
  {
    os<<p.str();
    return os;
  }
}
