/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_NTRACKS
#define INDETPHYSVALMONITORING_INDETPERFPLOT_NTRACKS
/**
 *  @file InDetPerfPlot_nTracks.h
 *  @author shaun roe
 **/

// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"

///Class to hold various counters used in RTT code
class InDetPerfPlot_nTracks: public InDetPlotBase {
public:
  InDetPerfPlot_nTracks(InDetPlotBase* pParent, const std::string& dirName);
  enum CounterCategory {
    ALL, SELECTED, TRUTH, TRUTH_MATCHED, N_COUNTERS
  };
  void fill(const unsigned int freq, const CounterCategory counter);
private:
  TH1* m_counters[N_COUNTERS];

  void initializePlots();
};

#endif
