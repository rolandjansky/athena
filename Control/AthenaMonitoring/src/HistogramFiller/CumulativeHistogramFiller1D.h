/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_CumulativeHistogramFiller1D_h
#define AthenaMonitoring_HistogramFiller_CumulativeHistogramFiller1D_h

#include "HistogramFiller1D.h"

namespace Monitored {
  /**
   * @brief Filler for 1D histograms filled in cummulative mode
   */
  class CumulativeHistogramFiller1D : public HistogramFiller1D {
  public:
    CumulativeHistogramFiller1D(TH1* hist, const HistogramDef& histDef) 
      : HistogramFiller1D(hist, histDef) {}
    
    virtual CumulativeHistogramFiller1D* clone() override { return new CumulativeHistogramFiller1D(*this); };

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
        unsigned bin = hist->FindBin(value);

        for (unsigned j = bin; j > 0; --j) {
          hist->AddBinContent(j);
        }

        ++i;
      }

      return i;  
    }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_CumulativeHistogramFiller1D_h */