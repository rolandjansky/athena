/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_CumulativeHistogramFiller1D_h
#define AthenaMonitoring_HistogramFiller_CumulativeHistogramFiller1D_h

#include "AthenaMonitoring/HistogramFiller/HistogramFiller1D.h"

namespace Monitored {
  /**
   * @brief filler for 1D histograms filled in cummulative mode
   */
  class CumulativeHistogramFiller1D : public HistogramFiller1D {
  public:
    CumulativeHistogramFiller1D(TH1* hist, const HistogramDef& histDef) 
      : HistogramFiller1D(hist, histDef) {}
    virtual unsigned fill() override;
    virtual CumulativeHistogramFiller1D* clone() override { return new CumulativeHistogramFiller1D(*this); };
  };
}

#endif /* AthenaMonitoring_HistogramFiller_CumulativeHistogramFiller1D_h */