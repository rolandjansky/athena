/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_BARRELFASTSIMDEDICATEDSD_H
#define LARG4FASTSIMULATION_BARRELFASTSIMDEDICATEDSD_H

// This is a sensitive detector.  It will do everything that a
// sensitive detector does.  We give this to the framework, sort
// of:  we DO want it to execute it's end-

#include "IFastSimDedicatedSD.h"
#include "LArG4Code/LArG4Identifier.h"

// Forward declarations
class EnergySpot;
class G4HCofThisEvent;
class LArVHitMerger;
class EMBDetectorManager;
class EMBAccordionDetails;
class GeoStraightAccSection;
class StoreGateSvc;

/// NEEDS DOCUMENTATION
class BarrelFastSimDedicatedSD : public IFastSimDedicatedSD
{

public:

  // Constructor:
  BarrelFastSimDedicatedSD(StoreGateSvc*);

  // Destructor:
  ~BarrelFastSimDedicatedSD() {}

  // ProcessHitsMethod
  void ProcessSpot(const EnergySpot & spot) override final;

private:

  const EMBDetectorManager  *m_embManager;
  const EMBAccordionDetails *m_accordionDetails;
  const GeoStraightAccSection *m_absorberSections;

  /// My LAr identifier
  LArG4Identifier m_larID;
};

#endif //LARG4FASTSIMULATION_BARRELFASTSIMDEDICATEDSD_H


