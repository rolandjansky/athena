/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoring_HistogramFiller_VecHistogramFiller1D_h
#define AthenaMonitoring_HistogramFiller_VecHistogramFiller1D_h

#include "AthenaMonitoring/HistogramFiller/HistogramFiller1D.h"

namespace Monitored {
  class VecHistogramFiller1D : public HistogramFiller1D {
  public:
    VecHistogramFiller1D(TH1* hist, const HistogramDef& histDef) 
      : HistogramFiller1D(hist, histDef) {}
    virtual unsigned fill() override;
    virtual VecHistogramFiller1D* clone() override { return new VecHistogramFiller1D(*this); };
  };
}

#endif /* AthenaMonitoring_HistogramFiller_VecHistogramFiller1D_h */