/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*file Bins_LessThan_NonZeroMedian.h author Justin Griffiths based of template dqm_algorithms::
BinsLessThanThreshold class author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_BINS_LESSTHANNONZEROMEDIAN_THRESHOLD_H
#define DQM_ALGORITHMS_BINS_LESSTHANNONZEROMEDIAN_THRESHOLD_H

#include "dqm_core/Algorithm.h"
#include <dqm_algorithms/BinThreshold.h>

namespace dqm_algorithms {


  struct Bins_LessThanNonZeroMedian_Threshold : public BinThreshold
  {
    Bins_LessThanNonZeroMedian_Threshold(): BinThreshold("LessThanNonZeroMedian") {};    
  };



} //dqm_algorithms

#endif // DQM_ALGORITHMS_BINS_LESSTHANNONZEROMEDIAN_THRESHOLD_H
