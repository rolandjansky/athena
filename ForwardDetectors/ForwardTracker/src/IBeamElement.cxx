/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ForwardTracker/IBeamElement.h"

#include <ostream>

namespace ForwardTracker {

  std::ostream& operator<<(std::ostream& os, const IBeamElement& be) { os << be.str(); return os; }
}
