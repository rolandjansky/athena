/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_IHistogramProvider_h
#define AthenaMonitoring_HistogramFiller_IHistogramProvider_h

#include "TNamed.h"

namespace Monitored {
  class IHistogramProvider {
  public: 
    virtual TNamed* histogram() = 0;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_IHistogramProvider_h */