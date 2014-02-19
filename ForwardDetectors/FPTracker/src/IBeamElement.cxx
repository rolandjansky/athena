/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/IBeamElement.h"
#include <ostream>
namespace FPTracker{
  IBeamElement::~IBeamElement(){}
  
  std::ostream& operator<<(std::ostream& os, const IBeamElement& ibe)
  {
    os<<ibe.str();
    return os;
  }
}
