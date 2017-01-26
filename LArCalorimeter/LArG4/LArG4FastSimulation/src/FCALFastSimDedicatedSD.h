/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_FCALFASTSIMDEDICATEDSD_H
#define LARG4FASTSIMULATION_FCALFASTSIMDEDICATEDSD_H

// This is a sensitive detector.  It will do everything that a
// sensitive detector does.  We give this to the framework, sort
// of:  we DO want it to execute it's end-

#include "IFastSimDedicatedSD.h"
#include "LArG4Code/LArG4Identifier.h"

class EnergySpot;
class LArVHitMerger;
class FCALDetectorManager;
class StoreGateSvc;

/// NEEDS DOCUMENTATION
class FCALFastSimDedicatedSD : public IFastSimDedicatedSD
{
public:

  // Constructor:
  FCALFastSimDedicatedSD(StoreGateSvc*);

  // Destructor:
  ~FCALFastSimDedicatedSD() {}

  // ProcessHitsMethod
  virtual void ProcessSpot(const EnergySpot & spot) override final;

private:

  FCALDetectorManager * m_fcalManager;

  /// My LAr identifier
  LArG4Identifier m_larID;
};

#endif //LARG4FASTSIMULATION_FCALFASTSIMDEDICATEDSD_H
