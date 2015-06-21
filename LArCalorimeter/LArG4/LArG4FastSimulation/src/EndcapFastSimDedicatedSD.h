/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4FASTSIMULATION_ENDCAPFASTSIMDEDICATEDSD_H
#define LARG4FASTSIMULATION_ENDCAPFASTSIMDEDICATEDSD_H

// This is a sensitive detector.  It will do everything that a
// sensitive detector does.  We give this to the framework, sort
// of:  we DO want it to execute it's end-

#include "G4AtlasTools/SensitiveDetectorBase.h"
#include "IFastSimDedicatedSD.h"

class EnergySpot;
class G4HCofThisEvent;
class LArVHitMerger;
class EMECDetectorManager;
class LArWheelCalculator;

class EndcapFastSimDedicatedSD : public SensitiveDetectorBase, virtual public IFastSimDedicatedSD
{

public:

  // Constructor:
  EndcapFastSimDedicatedSD(const std::string& type, const std::string& name, const IInterface *parent);

  // Destructor:
  ~EndcapFastSimDedicatedSD() {}

  StatusCode initialize() override final;

  virtual G4VSensitiveDetector* makeSD() override final { return (G4VSensitiveDetector*)NULL; };

  // ProcessHitsMethod
  void ProcessSpot(const EnergySpot & spot) override final;

  // End each event (do hit merger here)
  /** End of event collection of hits. Merging is done in this method.*/
  virtual StatusCode EndOfAthenaEvent() override final;

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf) override;

protected:
  virtual StatusCode retrieveLArHitMerger() final; //!< Method for lazy initialization of LArHitMerger. Temporary solution FIXME
  LArVHitMerger *m_hitMerger; //!< Hit merger for the combination of hits within a single detector area

private:

  EMECDetectorManager * m_emecManager;
  LArWheelCalculator  * m_innerWheelCalculatorPos;
  LArWheelCalculator  * m_innerWheelCalculatorNeg;
  LArWheelCalculator  * m_outerWheelCalculatorPos;
  LArWheelCalculator  * m_outerWheelCalculatorNeg;

};

#endif //LARG4FASTSIMULATION_ENDCAPFASTSIMDEDICATEDSD_H


