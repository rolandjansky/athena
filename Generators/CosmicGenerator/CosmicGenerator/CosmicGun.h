/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COSMICGENERATOR_COSMICGUN_H
#define COSMICGENERATOR_COSMICGUN_H

#include "CLHEP/Vector/LorentzVector.h"

class CosmicGun{
public:
  static CosmicGun* GetCosmicGun(void);
  CLHEP::HepLorentzVector GenerateEvent(void);
  CLHEP::HepLorentzVector GenerateVertex(void);
  void SetEnergyRange(float emin, float emax);
  void SetCosCut(float ctcut);
  void PrintLevel(int printevt,int printmod);
  int  GetMuonCharge(void);
  float InitializeGenerator();  // returns flux after all cuts in cm2/s
private:
  CosmicGun(void);
  static CosmicGun* s_mpointer;

  int   m_event;
  int   m_printevt, m_printmod;
  float m_emin, m_emax;
  float m_coscut;
};


#endif
