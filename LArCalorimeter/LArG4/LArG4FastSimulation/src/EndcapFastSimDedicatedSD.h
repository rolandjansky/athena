/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_ENDCAPFASTSIMDEDICATEDSD_H
#define LARG4FASTSIMULATION_ENDCAPFASTSIMDEDICATEDSD_H

// This is a sensitive detector.  It will do everything that a
// sensitive detector does.  We give this to the framework, sort
// of:  we DO want it to execute it's end-

#include "IFastSimDedicatedSD.h"
#include "LArG4Code/LArG4Identifier.h"

class EnergySpot;
class G4HCofThisEvent;
class LArVHitMerger;
class EMECDetectorManager;
class LArWheelCalculator;
class StoreGateSvc;

/// NEEDS DOCUMENTATION
class EndcapFastSimDedicatedSD : public IFastSimDedicatedSD
{

public:

  // Constructor:
  EndcapFastSimDedicatedSD(StoreGateSvc*);

  // Destructor:
  ~EndcapFastSimDedicatedSD() {}

  // ProcessHitsMethod
  void ProcessSpot(const EnergySpot & spot) override final;

private:

  EMECDetectorManager * m_emecManager;
  LArWheelCalculator  * m_innerWheelCalculatorPos;
  LArWheelCalculator  * m_innerWheelCalculatorNeg;
  LArWheelCalculator  * m_outerWheelCalculatorPos;
  LArWheelCalculator  * m_outerWheelCalculatorNeg;

  /// My LAr identifier
  LArG4Identifier m_larID;
};

#endif //LARG4FASTSIMULATION_ENDCAPFASTSIMDEDICATEDSD_H


