/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_READCONFIGDATA_H
#define FPTRACKER_READCONFIGDATA_H
#include "boost/shared_ptr.hpp"
#include <fstream>

namespace FPTracker{
  class ConfigData;
  bool readConfigData(boost::shared_ptr< std::ifstream >  confDir, ConfigData&);
}

#endif
