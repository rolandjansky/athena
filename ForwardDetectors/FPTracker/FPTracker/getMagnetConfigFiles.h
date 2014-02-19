/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_GETMAGNETCONFIGFILES_H
#define FPTRACKER_GETMAGNETCONFIGFILES_H

#include "FPTracker/FPTrackerConstants.h"
#include "boost/shared_ptr.hpp"
#include <string>
#include <map>


namespace FPTracker{
  boost::shared_ptr< std::ifstream >  getMagnetConfigFiles(const std::string& dir, int IP, int magVer, const Side& side);
}

#endif
