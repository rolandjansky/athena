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
      using namespace std;

      if (m_monVariables.size() != 1) {
        return 0;
      }

      unsigned i(0);
      auto hist = histogram();
      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      lock_guard<mutex> lock(*(this->m_mutex));

      for (auto value : valuesVector) {
        hist->AddBinContent(i, value);
        hist->SetEntries(hist->GetEntries() + value);

        ++i;
      }

      return i;
    }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_VecHistogramFiller1DWithOverflows_h */