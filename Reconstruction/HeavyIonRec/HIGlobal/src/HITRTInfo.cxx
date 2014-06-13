/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HITRTInfo.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HITRTInfo.h"

#include <vector>
#include <iostream>
using std::cout;using std::endl;
using std::string;


HITRTInfo::HITRTInfo(const std::vector<int>& OccupancyPerPart)
{
  // initialize given schema

  m_OccupancyPerPart.assign(OccupancyPerPart.begin(),OccupancyPerPart.end());
  // m_name = 0;
}
