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
    virtual unsigned fill() override;
    virtual HistogramFillerEfficiency* clone() override { return new HistogramFillerEfficiency(*this); };
  protected:
    virtual TH1* histogram() { return m_hist; } // Keep the compiler happy
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerEfficiency_h */