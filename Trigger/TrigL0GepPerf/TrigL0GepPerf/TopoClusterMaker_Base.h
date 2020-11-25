/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPOCLUSTERMAKER_BASE_H
#define TOPOCLUSTERMAKER_BASE_H

#include <map> 
#include <string>

#include "TrigL0GepPerf/CustomTopoCluster.h"
#include "TrigL0GepPerf/CustomCaloCell.h"

class TopoClusterMaker_Base
{
 public:

  virtual std::vector<CustomTopoCluster>  makeTopoCluster(const std::map<unsigned int,CustomCaloCell> &caloCellsMap) = 0;
  virtual std::string getName() const = 0;
  
  virtual ~TopoClusterMaker_Base() {}

};


#endif
