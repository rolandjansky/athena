// This file is really -*- C++ -*-.

/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                               
*/

#ifndef TRIGT1MUCTPIPHASE1_MUCTPIRESULTS_H
#define TRIGT1MUCTPIPHASE1_MUCTPIRESULTS_H

/*
  Simple holder for results of MUCTPI simulation
*/

#include "TrigT1Interfaces/MuCTPIL1Topo.h"
#include "TrigConfData/L1Threshold.h"

namespace LVL1MUCTPIPHASE1 {

  struct DAQData {

    DAQData(unsigned word, 
	    const std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> >& decisions)
    {
      dataWord=word;
      thresholdDecisions=decisions;
    }

    unsigned dataWord=0;
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > thresholdDecisions;
  };

  class MUCTPIResults
  {
  public:
    LVL1::MuCTPIL1Topo l1topoData;
    std::vector<unsigned int> ctp_words;
    std::vector<DAQData> daq_data;
  };
}

#endif
