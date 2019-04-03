/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_StaticHistogramProvider_h
#define AthenaMonitoring_HistogramFiller_StaticHistogramProvider_h

#include <memory>

#include "AthenaMonitoring/HistogramDef.h"
#include "AthenaMonitoring/IHistogramProvider.h"

#include "HistogramFactory.h"

namespace Monitored {
  /**
   * @brief Default implementation of IHistogramProvider interface
   * 
   * This provider implements standard way of handling ROOT objects in the code:
   * 1. Create and store ROOT object during provider initialization
   * 2. Return stored ROOT object if needed
   */
  class StaticHistogramProvider : public IHistogramProvider {
  public:
    /**
     * @brief Constructor
     * 
     * @param factory ROOT objects factory 
     * @param def Definition of a histogram to create
     */
    StaticHistogramProvider(std::shared_ptr<HistogramFactory> factory, const HistogramDef& def)
      : m_histogram(factory->create(def)) {}

    /**
     * @brief Getter of ROOT object 
     * 
     * @return ROOT object
     */
    TNamed* histogram() override {
      return m_histogram;
    }
  private:
    TNamed* m_histogram;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_IHistogramProvider_h */