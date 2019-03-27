/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_StaticHistogramProvider_h
#define AthenaMonitoring_HistogramFiller_StaticHistogramProvider_h

#include <memory>

#include "AthenaMonitoring/HistogramDef.h"
#include "AthenaMonitoring/HistogramFiller/HistogramFactory.h"

#include "AthenaMonitoring/HistogramFiller/IHistogramProvider.h"

namespace Monitored {
  class StaticHistogramProvider : public IHistogramProvider {
  public:
    StaticHistogramProvider(std::shared_ptr<HistogramFactory> factory, const HistogramDef& def)
      : IHistogramProvider(), m_histogram(factory->create(def)) {}

    TNamed* histogram() override {
      return m_histogram;
    }
  private:
    TNamed* m_histogram;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_IHistogramProvider_h */