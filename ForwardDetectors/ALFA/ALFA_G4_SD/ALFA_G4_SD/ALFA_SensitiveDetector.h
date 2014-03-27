/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_SensitiveDetector_h
#define ALFA_SensitiveDetector_h

#include "ALFA_SimEv/ALFA_HitCollection.h"
#include "ALFA_SimEv/ALFA_ODHitCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "globals.hh"

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

#include <string> 

class ALFA_SensitiveDetector : public FADS::FadsSensitiveDetector {

public:
  
  ALFA_SensitiveDetector(std::string);
  ~ALFA_SensitiveDetector();
  
  void Initialize (G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*); 
  void EndOfEvent (G4HCofThisEvent*);
  
private:

  int hitID;
  int trackID; //DC 2-29-04 use it as barcode
  int particleEncoding;
  double kineticEnergy;
  double energyDeposit;
  double preStepX;
  double preStepY;
  double preStepZ;
  double postStepX;
  double postStepY;
  double postStepZ;
  double globalTime;
    
  int sign_fiber;
  int OD_side;  
  int n_plate;
  int n_fiber;
  int n_station;
  
  int eventNumber;
  int numberOfHits;
  int numberOfODHits;  

  ALFA_HitCollection* pHitCollection;
  ALFA_ODHitCollection* pODHitCollection;  
  
  AthenaHitsCollectionHelper m_hitCollHelp;
  
  int pos1, pos2;
  
  int num[3];
};

#endif
