/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_nTracks.cxx
 * @author shaun roe
 **/

#include "InDetPerfPlot_nTracks.h"


InDetPerfPlot_nTracks::InDetPerfPlot_nTracks(InDetPlotBase* pParent, const std::string& sDir) : InDetPlotBase(pParent,
                                                                                                              sDir),
  m_counters{0} {
  // nop
}

void
InDetPerfPlot_nTracks::initializePlots() {
  const bool prependDirectory(false);
  SingleHistogramDefinition hd = retrieveDefinition("ntrack");

  m_counters[ALLRECO] = Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("ntracksel");
  m_counters[SELECTEDRECO] = Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("nparticle");
  m_counters[ALLTRUTH] = Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);
  hd = retrieveDefinition("num_truthmatch_match");
  m_counters[MATCHEDRECO] =
    Book1D(hd.name, hd.allTitles, hd.nBinsX, hd.xAxis.first, hd.xAxis.second, prependDirectory);

  book(m_ntracks_vs_mu,"ntracks_vs_mu");
  book(m_ntracks_vs_nvertices,"ntracks_vs_nvertices");
  //book(m_ntracks_vs_nmatchedvertices,"ntracks_vs_nmatchedvertices");
  //book(m_ntracks_vs_nmergedvertices,"ntracks_vs_nmergedvertices");
  //book(m_ntracks_vs_nsplitvertices,"ntracks_vs_nsplitvertices");
  //book(m_ntracks_vs_nfakevertices,"ntracks_vs_nfakevertices");
 



}

void
InDetPerfPlot_nTracks::fill(const unsigned int freq, const CounterCategory counter) {
  if (counter < N_COUNTERS) {
    fillHisto((m_counters[counter]), freq);
  }
}

void InDetPerfPlot_nTracks::fill(const unsigned int ntracks, const unsigned int muu, const unsigned int nvertices) {

  fillHisto(m_ntracks_vs_mu, muu, ntracks);
  fillHisto(m_ntracks_vs_nvertices, nvertices, ntracks);
 // fillHisto(m_ntracks_vs_nmatchedvertices,nvertices, ntracks);
 // fillHisto(m_ntracks_vs_nmergedvertices,nvertices, ntracks);
 // fillHisto(m_ntracks_vs_nsplitvertices,nvertices, ntracks);
 // fillHisto(m_ntracks_vs_nfakevertices,nvertices, ntracks);

}
