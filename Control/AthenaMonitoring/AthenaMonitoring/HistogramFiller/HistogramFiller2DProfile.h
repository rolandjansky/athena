/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFiller2DProfile_h
#define AthenaMonitoring_HistogramFiller_HistogramFiller2DProfile_h

#include "TProfile2D.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief filler for profile 2D histogram
   */
  class HistogramFiller2DProfile : public HistogramFiller {
  public:
    HistogramFiller2DProfile(TProfile2D* hist, const HistogramDef& histDef)
      : HistogramFiller(hist, histDef) {};
    virtual unsigned fill() override;
    virtual HistogramFiller2DProfile* clone() override { return new HistogramFiller2DProfile(*this); };
  protected:
    virtual TProfile2D* histogram() override { return static_cast<TProfile2D*>(m_hist); }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFiller2DProfile_h */