/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_TWISSFILE_H
#define FORWARDTRACKER_TWISSFILE_H

#include "ConfigData.h"
#include "ForwardTrackerConstants.h"

#include "boost/shared_ptr.hpp"

namespace ForwardTracker {

  boost::shared_ptr<std::ifstream> TwissFile(const ConfigData&, const Side&);

  double GetBeamEnergy (std::ifstream&);
  void   TwissFilePrint(std::ifstream&, double endMarker);
}

#endif
