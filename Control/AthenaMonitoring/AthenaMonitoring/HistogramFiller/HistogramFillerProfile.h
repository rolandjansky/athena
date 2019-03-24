/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h
#define AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h

#include "TProfile.h"

#include "AthenaMonitoring/HistogramFiller/HistogramFiller.h"

namespace Monitored {
  /**
   * @brief filler for profile 1d histogram 
   */
  class HistogramFillerProfile : public HistogramFiller {
  public:
    HistogramFillerProfile(TProfile* hist, const HistogramDef& histDef)
      : HistogramFiller(hist, histDef) {};
    virtual unsigned fill() override;
    virtual HistogramFillerProfile* clone() override { return new HistogramFillerProfile(*this); };
  protected:
    virtual TProfile* histogram() override { return static_cast<TProfile*>(m_hist); }
  };
}

#endif /* AthenaMonitoring_HistogramFiller_HistogramFillerProfile_h */