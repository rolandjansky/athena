/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHitMaker
// Created by F. Mazzucato September 2002 
// Modified 30-Apr-2003 Bill Seligman

// Convert an EnergySpot into a LArG4-type hit.

#ifndef __LArHitMaker_h__
#define __LArHitMaker_h__

#include "G4TouchableHandle.hh"
#include "globals.hh"

// Forward declarations
class EnergySpot;
class G4StepPoint;
class G4Step;
class G4Navigator;

class LArHitMaker
{
public:
  LArHitMaker();
  virtual ~LArHitMaker();
  virtual void make(const EnergySpot& spot);

private:  
  G4Step*           fFakeStep;
  G4StepPoint*      fFakePreStepPoint;
  G4StepPoint*      fFakePostStepPoint;
  G4TouchableHandle fTouchableHandle;
  G4Navigator*      fpNavigator;
  G4bool            fNaviSetup;

  G4String          fBaseName;
};

#endif
