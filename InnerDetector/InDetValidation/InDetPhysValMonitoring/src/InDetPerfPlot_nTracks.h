/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
class TH1;
class TH2;

///Class to hold various counters used in RTT code
class InDetPerfPlot_nTracks: public InDetPlotBase {
public:
  InDetPerfPlot_nTracks(InDetPlotBase* pParent, const std::string& dirName);
  enum CounterCategory {
    ALLRECO, SELECTEDRECO, ALLTRUTH, SELECTEDTRUTH, ALLASSOCIATEDTRUTH, MATCHEDRECO, N_COUNTERS
  };
  void fill(const unsigned int freq, const CounterCategory counter, float weight=1.0);
  void fill(const unsigned int ntracks, const unsigned int mu, const unsigned int nvertices, float weight=1.0);

private:
  TH1* m_counters[N_COUNTERS];
  TH2* m_ntracks_vs_mu{};
  TH2* m_ntracks_vs_nvertices{};
  void initializePlots();
};

#endif
