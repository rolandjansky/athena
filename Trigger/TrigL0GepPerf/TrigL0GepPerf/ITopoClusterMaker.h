/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGL0GEPPERF_ITOPOCLUSTERMAKER_H
#define TRIGL0GEPPERF_ITOPOCLUSTERMAKER_H

#include <map> 
#include <string>

#include "TrigL0GepPerf/CustomTopoCluster.h"
#include "TrigL0GepPerf/CustomCaloCell.h"

namespace Gep{
  class ITopoClusterMaker
  {
  public:
    
    virtual std::vector<Gep::CustomTopoCluster>  makeTopoCluster(const std::map<unsigned int,Gep::CustomCaloCell> &caloCellsMap) = 0;
    virtual std::string getName() const = 0;
    
    virtual ~ITopoClusterMaker() {}
    
  };
}


#endif
