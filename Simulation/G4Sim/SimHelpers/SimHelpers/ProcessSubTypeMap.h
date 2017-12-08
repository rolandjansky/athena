/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ProcessSubTypeMap_H
#define ProcessSubTypeMap_H 1

#include "G4VProcess.hh"
#include <map>

//=======================================================
//
// Following enum will be replaced with Geant4 ones
// when Atlas migrates to Geant4 9.2 or later
//
enum G4EmProcessSubType 
{ 
    fSubTypeNotDefined = -1, 
    fCoulombScattering = 1, 
    fIonisation = 2, 
    fBremsstrahlung = 3, 
    fPairProdByCharged = 4,
    fAnnihilation = 5, 
    fAnnihilationToMuMu = 6,
    fAnnihilationToHadrons = 7,
  
    fRayleigh = 11,
    fPhotoElectricEffect = 12,
    fComptonScattering = 13,
    fGammaConversion = 14,
 
    fCerenkov = 21,
    fScintillation = 22,
    fSynchrotronRadiation = 23,
    fTransitionRadiation = 24,
  
    fOpAbsorption = 31,
    fOpBoundary = 32,
    fOpRayleigh = 33,
    fOpWLS = 34
};

enum G4HadronicProcessType
{
  fHadronElastic =   111,
  fHadronInelastic = 121,
  fCapture =         131,
  fFission =         141,
  fHadronAtRest =    151,
  fChargeExchange =  161
};

enum G4DecayProcessType
{
  DECAY =              201,
  DECAY_WithSpin =     202,
  DECAY_PionMakeSpin = 203,
  DECAY_Unknown =      211,
  DECAY_External =     231
};

//==============================================================

class ProcessSubTypeMap
{
 public:
  ProcessSubTypeMap();
  ~ProcessSubTypeMap();

  G4int GetProcessSubType(const G4VProcess*);

 private:
  std::map<const G4VProcess*,G4int> m_map;
};

#endif

