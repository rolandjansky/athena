/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/IParticle.h"

namespace ForwardTracker {
  
  std::ostream& operator<<(std::ostream& os, const IParticle& p) { os << p.str(); return os; }
}
