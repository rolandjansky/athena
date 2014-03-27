/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_STRIP_SD
#define ZDC_STRIP_SD

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

class AthenaHitsCollectionHelper;

class ZDC_StripSD : public FADS::FadsSensitiveDetector
{

 public:
  
 ZDC_StripSD(std::string name): FADS::FadsSensitiveDetector(name) {};
  ~ZDC_StripSD() {};
  
  void   Initialize (G4HCofThisEvent *HCE);
  G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
  void   EndOfEvent (G4HCofThisEvent *HCE);
  
 private:

  float    Edep_Cherenkov_Strip[2][4]; 
  int   NPhoton_Cherenkov_Strip[2][4]; 
  
  AthenaHitsCollectionHelper  m_HitCollHelp;
};

#endif
