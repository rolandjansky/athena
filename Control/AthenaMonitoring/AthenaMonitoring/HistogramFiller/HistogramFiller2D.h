/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller2D_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller2D_h

#include "TH2.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief filler for plain 2D histogram
   */
  class HistogramFiller2D : public HistogramFiller {
  public:
    HistogramFiller2D(TH2* hist, const HistogramDef& histDef)
      : HistogramFiller(hist, histDef) {};
    virtual unsigned fill() override;
    virtual HistogramFiller2D* clone() override { return new HistogramFiller2D(*this); };
  protected:
    virtual TH2* histogram() override { return static_cast<TH2*>(m_hist); }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller2D_h */