/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_nTracks.cxx
 * @author shaun roe
**/

#include "InDetPerfPlot_nTracks.h"


InDetPerfPlot_nTracks::InDetPerfPlot_nTracks(PlotBase* pParent, const std::string & sDir):PlotBase(pParent, sDir),
m_counters{0} {
  //nop
}


void 
InDetPerfPlot_nTracks::initializePlots() {
    m_counters[ALL] = Book1D("ntrack","Number of Tracks;Num. tracks",200,0,1000,false);
    m_counters[SELECTED] = Book1D("ntracksel","Number of Selected Tracks;Num. tracks",200,0,1000,false);
    m_counters[TRUTH] = Book1D("nparticle", "Number of Truth Particles;Num. truth particles",200,0,2000,false);
    m_counters[TRUTH_MATCHED] = Book1D("num_truthmatch_match","num_truthmatch_match; evi match	",10,0,10,false);
}

void 
InDetPerfPlot_nTracks::fill(const unsigned int freq, const CounterCategory counter){
	if (counter<N_COUNTERS) (m_counters[counter])->Fill(freq);
}



