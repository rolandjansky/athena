/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_VecHistogramFiller1DWithOverflows_h
#define AthenaMonitoring_HistogramFiller_VecHistogramFiller1DWithOverflows_h

#include "HistogramFiller1D.h"

namespace Monitored {
  class VecHistogramFiller1DWithOverflows : public HistogramFiller1D {
  public:
    VecHistogramFiller1DWithOverflows(TH1* hist, const HistogramDef& histDef) 
      : HistogramFiller1D(hist, histDef) {}

    virtual VecHistogramFiller1DWithOverflows* clone() override { return new VecHistogramFiller1DWithOverflows(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      for (unsigned i = 0; i < std::size(valuesVector); ++i) {
        auto value = valuesVector[i];
        m_histogram->AddBinContent(i, value);
        m_histogram->SetEntries(m_histogram->GetEntries() + value);
      }

      return std::size(valuesVector);
    }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_VecHistogramFiller1DWithOverflows_h */