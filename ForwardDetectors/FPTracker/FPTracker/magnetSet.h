/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_MAGNETSET_H
#define FPTRACKER_MAGNETSET_H


#include "Magnet.h"
#include "FPTrackerConstants.h"
#include <string>

namespace FPTracker{
  class ConfigData;
  Magnet::Container_t  magnetSet(
				 const ConfigData&, 
				 const Side& side,
				 int magversion,
				 boost::shared_ptr< std::ifstream> magfile);
}
#endif
