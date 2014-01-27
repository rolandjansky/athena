/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_MAGNETFACTORY_H
#define FORWARDTRACKER_MAGNETFACTORY_H

#include "ForwardTrackerConstants.h"
#include "Magnet.h"

namespace ForwardTracker {

  Magnet::ConstPtr_t MagnetFactory(double       x,
				   double       y,
				   double       z,
				   double       strength,
				   double       length,
				   std::string  aperType,
				   double       A1,
				   double       A2,
				   double       A3,
				   double       A4,
				   double       beamEnergy,
				   Side         side,
				   Magnet::Type type);
}

#endif
