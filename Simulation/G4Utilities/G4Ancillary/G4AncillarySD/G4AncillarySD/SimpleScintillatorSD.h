/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIMPLESCINTILLATORSD_H
#define SIMPLESCINTILLATORSD_H

// Base class
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;
class G4LogicalVolume;

class StoreGateSvc;

#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "G4AncillarySimEvent/SimpleScintillatorHitCollection.h" // Typedef, not a class; forward declaration won't work

class SimpleScintillatorSD : public FADS::FadsSensitiveDetector {

 public:

  SimpleScintillatorSD( G4String );
  ~SimpleScintillatorSD();
  void Initialize( G4HCofThisEvent * );
  G4bool ProcessHits( G4Step *, G4TouchableHistory * );
  void EndOfEvent( G4HCofThisEvent * );

private:

  StoreGateSvc * m_sgSvc;
  AthenaHitsCollectionHelper m_hitCollHelp;
  SimpleScintillatorHitCollection * hitCollection;

};

#endif
