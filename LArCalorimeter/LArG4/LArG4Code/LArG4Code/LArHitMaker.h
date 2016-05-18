/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHitMaker
// Created by F. Mazzucato September 2002 
// Modified 30-Apr-2003 Bill Seligman

// Convert an EnergySpot into a LArG4-type hit.

#ifndef LARG4CODE_LARHITMAKER_h
#define LARG4CODE_LARHITMAKER_h

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
  G4Step*           m_fakeStep;
  G4StepPoint*      m_fakePreStepPoint;
  G4StepPoint*      m_fakePostStepPoint;
  G4TouchableHandle m_touchableHandle;
  G4Navigator*      m_pNavigator;
  G4bool            m_naviSetup;

  G4String          m_baseName;
};

#endif
