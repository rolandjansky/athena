/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerEfficiency_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerEfficiency_h

#include "TEfficiency.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief filler for TEfficiency histograms
   */
  class HistogramFillerEfficiency : public HistogramFiller {
  public:
    HistogramFillerEfficiency(TEfficiency* eff, const HistogramDef& histDef)
      : HistogramFiller(eff, histDef) {};

    virtual HistogramFillerEfficiency* clone() override { return new HistogramFillerEfficiency(*this); };

    virtual unsigned fill() override {
      if (m_monVariables.size() != 2) {
        return 0;
      }

      unsigned i(0);
      auto hist = histogram();
      auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
      auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
      std::lock_guard<std::mutex> lock(*(this->m_mutex));

      for (i = 0; i < valuesVector1.size(); ++i) {
        hist->Fill(valuesVector1[i],valuesVector2[i]);
      }
      
      return i;
    }
  protected:
    virtual TEfficiency* histogram() override { return static_cast<TEfficiency*>(m_hist); }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerEfficiency_h */