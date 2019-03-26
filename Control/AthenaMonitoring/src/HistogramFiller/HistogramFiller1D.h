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
    HistogramFiller1D(TH1* const hist, const HistogramDef& histDef)
      : HistogramFiller(histDef), m_histogram(hist) {}

    HistogramFiller1D(const HistogramFiller1D& hf) 
      : HistogramFiller(hf), m_histogram(hf.m_histogram) {}
    
    HistogramFiller1D* clone() override { return new HistogramFiller1D(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 1) {
        return 0;
      }

      auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      for (auto value : valuesVector) {
        m_histogram->Fill(value);
      }

      return std::size(valuesVector);
    } 
  protected:
    TH1* const m_histogram;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller1D_h */