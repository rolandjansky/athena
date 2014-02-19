/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_NULLBENDER_H
#define FPTRACKER_NULLBENDER_H
#include "IBender.h"
#include "Point.h"
namespace FPTracker{
  class IParticle;
 class NullBender: public IBender{
  public:
    void bend(IParticle&) const {
      return;
    }
    std::string label() const {return "NullBender";}
  };
}
#endif
