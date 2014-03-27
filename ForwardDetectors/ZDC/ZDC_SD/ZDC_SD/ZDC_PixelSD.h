/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_PIXEL_SD
#define ZDC_PIXEL_SD

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

class AthenaHitsCollectionHelper;

class ZDC_PixelSD : public FADS::FadsSensitiveDetector
{

 public:
  
 ZDC_PixelSD(std::string name): FADS::FadsSensitiveDetector(name) {};
  ~ZDC_PixelSD() {};
  
  void   Initialize (G4HCofThisEvent *HCE);
  G4bool ProcessHits(G4Step *aStep, G4TouchableHistory* ROhist);
  void   EndOfEvent (G4HCofThisEvent *HCE);
  
private:
  
  double Edep_Cherenkov_Pixel   [2][2][80];
  int    NPhoton_Cherenkov_Pixel[2][2][80];
  
  // The first index is [A_side/C_side] second is [EMXY/HMXY] third is [0...79(63)] channel No  
  // I keep 80 channels in the HMXY and add them up later during digitization
  
  AthenaHitsCollectionHelper m_HitCollHelp;
};

#endif
