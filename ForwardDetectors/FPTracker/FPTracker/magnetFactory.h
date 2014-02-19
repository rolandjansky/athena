/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_MAGNETFACTORY_H
#define FPTRACKER_MAGNETFACTORY_H

#include "FPTrackerConstants.h"
#include "Magnet.h"
namespace FPTracker{
  class Magnet;
  Magnet::ConstPtr_t magnetFactory(
				   double x,
				   double y,
				   double center,
				   double strength,
				   double length,
				   int apertype,
				   double A1,
				   double A2,
				   double A3,
				   double A4,
				   double X,
				   double pbeam0,
				   Side side,
				   Magnet::Type type);
}
#endif
