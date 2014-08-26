/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArG4H6BeamSD_h
#define LArG4H6BeamSD_h 1

#include "LArG4TBSimEvent/LArG4H6FrontHit.h"
#include "LArG4TBSimEvent/LArG4H6FrontHitCollection.h"

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

// #define ARRSIZE plit src/0 
#define NUM_MWPC 10

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class StoreGateSvc;
class IMessageSvc;

class LArG4H6BeamSD : public FADS::FadsSensitiveDetector
{
  public:
      LArG4H6BeamSD(G4String name);
      ~LArG4H6BeamSD();
//
      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*,G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);
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
      LArG4H6FrontHitCollection* m_FrontCollection;

      StoreGateSvc *m_storeGate;
      IMessageSvc *m_msgSvc;

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

