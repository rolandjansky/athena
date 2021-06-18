/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_IJETMAKER_H
#define TRIGL0GEPPERF_IJETMAKER_H

#include <string>

#include "TrigL0GepPerf/CustomJet.h"
#include "TrigL0GepPerf/CustomTopoCluster.h"

namespace Gep{
  class IJetMaker
  {
  public:
    
    virtual std::vector<Gep::CustomJet> makeJet(const std::vector<Gep::CustomTopoCluster> &clusters) = 0;
    virtual std::string getName() const = 0;
    
    virtual ~IJetMaker() {}
    
  };
}

#endif
