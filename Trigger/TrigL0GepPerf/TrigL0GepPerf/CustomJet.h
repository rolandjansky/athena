/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_CUSTOMJET_H
#define TRIGL0GEPPERF_CUSTOMJET_H

#include "TLorentzVector.h"

namespace Gep{
  struct CustomJet
  {
   
    TLorentzVector vec;    
    float radius;
    int nConstituents;
    
  };
}

#endif //TRIGL0GEPPERF_CUSTOMJET_H
