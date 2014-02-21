/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _IFastSimDedicatedSD_h_
#define _IFastSimDedicatedSD_h_
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

// This is the interface for the fast simulation dedicated sensitive detector.

class EnergySpot;
class G4HCofThisEvent;
class LArVHitMerger;

class IFastSimDedicatedSD : public FADS::FadsSensitiveDetector {

 public:

  // Constructor:
 IFastSimDedicatedSD(const std::string & name);

 // Destructor:
 virtual ~IFastSimDedicatedSD() {};

 // ProcessHitsMethod
 /** Process a single energy spot from a frozen shower.
 The appropriate region of the sensitive detector is calculated and a LArIdentifier is constructed*/
 virtual void ProcessSpot(const EnergySpot & spot) = 0;
 
 // Initialize each event.  
 /** Initialization of each event*/
 virtual void Initialize(G4HCofThisEvent *);

 // End each event (do hit merger here)
 /** End of event collection of hits. Merging is done in this method.*/
 virtual void EndOfEvent(G4HCofThisEvent *);

 protected:
 
 LArVHitMerger *m_hitMerger; //!< Hit merger for the combination of hits within a single detector area

};

#endif
