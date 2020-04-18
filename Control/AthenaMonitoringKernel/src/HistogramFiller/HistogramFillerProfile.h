/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/  
  
#ifndef AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h
#define AthenaMonitoringKernel_HistogramFiller_HistogramFillerProfile_h

#include "TProfile.h"

#include "HistogramFiller2D.h"
#include "HistogramFillerRebinable.h"
#include "HistogramFillerUtils.h"

namespace Monitored {

  /// TProfile filler
  typedef HistogramFiller2DGeneric<TProfile> HistogramFillerProfile;
  /// TProfile filler with rebinable x-axis
  typedef HistogramFillerRebinableAxis<HistogramFillerProfile, Axis::X> HistogramFillerProfileRebinable;
}

#endif
