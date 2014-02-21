/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _FCALFastSimDedicatedSD_h_
#define _FCALFastSimDedicatedSD_h_

// This is a sensitive detector.  It will do everything that a
// sensitive detector does.  We give this to the framework, sort
// of:  we DO want it to execute it's end-

#include "LArG4FastSimulation/IFastSimDedicatedSD.h"

class EnergySpot;

class FCALFastSimDedicatedSD : public IFastSimDedicatedSD
{

 public:

  // Constructor:
 FCALFastSimDedicatedSD();

 // Destructor:
 virtual ~FCALFastSimDedicatedSD();

 // ProcessHitsMethod
 virtual void ProcessSpot(const EnergySpot & spot);

 private:

 class Clockwork;
 Clockwork *c;

 FCALFastSimDedicatedSD (const FCALFastSimDedicatedSD&);
 FCALFastSimDedicatedSD operator= (const FCALFastSimDedicatedSD&);
};

#endif


