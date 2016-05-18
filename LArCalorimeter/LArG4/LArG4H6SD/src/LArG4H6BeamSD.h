/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4H6BeamSD_h
#define LArG4H6BeamSD_h 1

// Base class
#include "G4VSensitiveDetector.hh"

// Hit colleciton
#include "StoreGate/WriteHandle.h"
#include "LArG4TBSimEvent/LArG4H6FrontHit.h"
#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"

// #define ARRSIZE plit src/0
#define NUM_MWPC 10

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class LArG4H6BeamSD : public G4VSensitiveDetector
{
  public:
      LArG4H6BeamSD(G4String name, G4String colname);
      ~LArG4H6BeamSD() {}
//
      void Initialize(G4HCofThisEvent*) override final;
      G4bool ProcessHits(G4Step*,G4TouchableHistory*) override final;
      void clear(){};
      void DrawAll(){};
      void PrintAll(){};
      inline float GetWDIM() { return WDIM;}
      inline float GetDW() {return DW;}
      inline float GetBPCWDIM() { return BPC_WDIM;}
      inline float GetBPCDW() {return BPC_DW;}
      inline float GetBPCOWDIM() { return BPC_WDIM;}
      inline float GetBPCODW() {return BPC_DW;}

  private:
      SG::WriteHandle<LArG4H6FrontHitCollection> m_FrontCollection;

// MWPC half width and wires step
      static const float WDIM;
      static const float DW;
// BPC half width and wires step
      static const float BPC_WDIM;
      static const float BPC_DW;
// OLD BPC half width and wires step
      static const float BPCO_WDIM;
      static const float BPCO_DW;
};
#endif
