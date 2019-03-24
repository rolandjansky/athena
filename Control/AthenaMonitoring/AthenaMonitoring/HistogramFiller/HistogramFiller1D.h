/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller1D_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller1D_h

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief filler for plain 1D histograms
   */
  class HistogramFiller1D : public HistogramFiller {
  public: 
    HistogramFiller1D(TH1* hist, const HistogramDef& histDef)
      : HistogramFiller(hist, histDef) {}
    virtual unsigned fill() override;
    HistogramFiller1D* clone() override { return new HistogramFiller1D(*this); };
  protected:
    virtual TH1* histogram() override { return m_hist; }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller1D_h */