/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_GETCONFIGFILE_H
#define FPTRACKER_GETCONFIGFILE_H

#include <string>
#include <fstream>
#include "boost/shared_ptr.hpp"

namespace FPTracker{
    boost::shared_ptr< std::ifstream>  getConfigFile(const std::string& dir, const std::string& fn);
}

#endif
