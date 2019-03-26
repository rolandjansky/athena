/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerEfficiency_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerEfficiency_h

#include "TEfficiency.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief Filler for TEfficiency graphs
   */
  class HistogramFillerEfficiency : public HistogramFiller {
  public:
    HistogramFillerEfficiency(TEfficiency* const eff, const HistogramDef& histDef)
      : HistogramFiller(histDef), m_histogram(eff) {};

    HistogramFillerEfficiency(const HistogramFillerEfficiency& hf) 
      : HistogramFiller(hf), m_histogram(hf.m_histogram) {}

    virtual HistogramFillerEfficiency* clone() override { return new HistogramFillerEfficiency(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 2) {
        return 0;
      }

      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      for (unsigned i = 0; i < std::size(valuesVector1); ++i) {
        m_histogram->Fill(valuesVector1[i],valuesVector2[i]);
      }
      
      return std::size(valuesVector1);
    }
  protected:
    TEfficiency* const m_histogram;
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerEfficiency_h */