/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_CumulativeHistogramFiller1D_h
#define AthenaMonitoringKernel_HistogramFiller_CumulativeHistogramFiller1D_h

#include "HistogramFiller1D.h"

namespace Monitored {
  /**
   * @brief Filler for 1D histograms filled in cummulative mode
   */
  class CumulativeHistogramFiller1D : public HistogramFiller1D {
  public:
    CumulativeHistogramFiller1D(const HistogramDef& definition, std::shared_ptr<IHistogramProvider> provider)
      : HistogramFiller1D(definition, provider) {}

    virtual CumulativeHistogramFiller1D* clone() override {
      return new CumulativeHistogramFiller1D( *this );
    }

    
    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      unsigned i(0);
      auto histogram = this->histogram<TH1>();
      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      for (auto value : valuesVector) {
        unsigned bin = histogram->FindBin(value);

        for (unsigned j = bin; j > 0; --j) {
          histogram->AddBinContent(j);
        }

        ++i;
      }

      return i;  
    }
  };
}

#endif /* AthenaMonitoringKernel_HistogramFiller_CumulativeHistogramFiller1D_h */
