/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArG4H62004MultSD_H__
#define __LArG4H62004MultSD_H__

//#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "LArG4SD/LArG4MultSD.h"
#include <vector>

// Forward declarations.
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class IMessageSvc;

//class LArG4H62004MultSD : public FADS::FadsSensitiveDetector 
class LArG4H62004MultSD : public LArG4MultSD 
{
public:
  LArG4H62004MultSD(G4String name);
  virtual ~LArG4H62004MultSD();
  
  // The required functions for all sensitive detectors:
//  virtual void Initialize(G4HCofThisEvent* HCE);
//  virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* ROhist);
//  virtual void EndOfEvent(G4HCofThisEvent* HCE);

  // Add a sensitive detector 
//  void addSD(FadsSensitiveDetector* sd);

  // Accessor methods
//  inline unsigned int size() { return m_sdList.size(); }
//  inline FadsSensitiveDetector* getSD(unsigned int i) { return i<m_sdList.size() ? m_sdList[i]: 0; }

private:
  // The name of the sensitive detector.
//  G4String m_detectorName;

  // Pointer to the message service
//  IMessageSvc* m_msgSvc;

  // List of SD pointers
//  std::vector< FadsSensitiveDetector* > m_sdList;
};

#endif // __LArG4H62004MultSD_H__
