/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4H6WarmTCSD.h

#ifndef __LArG4H6WarmTCSD_H__
#define __LArG4H6WarmTCSD_H__

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "LArG4TBSimEvent/LArG4H6WarmTCHit.h"
#include "LArG4TBSimEvent/LArG4H6WarmTCHitCollection.h"

#include "LArG4Code/CalibrationSensitiveDetector.h"

#include <map>

class StoreGateSvc;
class IMessageSvc;

class LArG4H6WarmTCSD : public FADS::FadsSensitiveDetector 
{
 public:

  // Constructor and destructor.
  LArG4H6WarmTCSD(G4String name);
  ~LArG4H6WarmTCSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*,G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  void clear(){};
  void DrawAll(){};
  void PrintAll(){};


  inline LArG4H6WarmTCHitCollection* GetColl(){ return m_Collection;}

private:

  bool m_isX;
  bool m_isABS;
  bool m_isCalib;
  std::map<int,LArG4H6WarmTCHit*> m_hits;
  typedef std::map<int,LArG4H6WarmTCHit*>::iterator hitIt;
  typedef std::map<int,LArG4H6WarmTCHit*>::value_type hitPair;

   LArG4H6WarmTCHitCollection* m_Collection;
   LArG4::CalibrationSensitiveDetector* m_CalibSD;

   StoreGateSvc *m_storeGate;
   IMessageSvc *m_msgSvc;

};

#endif
