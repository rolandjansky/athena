/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller1D_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller1D_h

#include "TH1.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for plain 1D histograms
   */
  class HistogramFiller1D : public HistogramFiller {
  public: 
    HistogramFiller1D(TH1* hist, const HistogramDef& histDef)
      : HistogramFiller(hist, histDef) {}
    
    HistogramFiller1D* clone() override { return new HistogramFiller1D(*this); };

    virtual unsigned fill() override {
      using namespace std;

      if (m_monVariables.size() != 1) {
        return 0;
      }

      unsigned i(0);
      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      lock_guard<mutex> lock(*(this->m_mutex));

      for (auto value : valuesVector) {
        histogram()->Fill(value);
        ++i;
      }

      return i;
    } 
  protected:
    virtual TH1* histogram() override { return static_cast<TH1*>(m_hist); }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller1D_h */