/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_IHistogramProvider_h
#define AthenaMonitoringKernel_HistogramFiller_IHistogramProvider_h

#include "TNamed.h"

namespace Monitored {
  /**
   * @brief Interface of the source of ROOT objects for HistogramFillers
   */
  class IHistogramProvider {
  public:
    virtual ~IHistogramProvider() = default;

    /**
     * @brief Getter of ROOT object 
     * 
     * @return ROOT object
     */
    virtual TNamed* histogram() = 0;
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_IHistogramProvider_h */
