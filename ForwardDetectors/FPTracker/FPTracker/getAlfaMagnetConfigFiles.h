/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_GETALFAMAGNETCONFIGFILES_H
#define FPTRACKER_GETALFAMAGNETCONFIGFILES_H

#include "FPTracker/FPTrackerConstants.h"
#include "boost/shared_ptr.hpp"
#include <string>
#include <map>


namespace FPTracker{
  boost::shared_ptr< std::ifstream >  getAlfaMagnetConfigFiles(const std::string& dir, const Side& side);
}

#endif
