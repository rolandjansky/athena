/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBECBeamChCalculator
#include "LArG4TBECBeamChCalculator.h"

#include "G4ThreeVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"

#include "globals.hh"

LArG4TBECBeamChCalculator::LArG4TBECBeamChCalculator(void)
  : m_number (-1),
    m_x (0),
    m_y (0)
{
  G4cout << "LArG4TBECBeamChCalculator ready" << G4endl;
}

G4bool LArG4TBECBeamChCalculator::Process(const G4Step* step)
{
  G4StepPoint* pre_step_point = step->GetPreStepPoint();
  G4StepPoint* post_step_point = step->GetPostStepPoint();

  G4ThreeVector startPoint = pre_step_point->GetPosition();
  G4ThreeVector endPoint   = post_step_point->GetPosition();
  G4ThreeVector p = (startPoint + endPoint) * 0.5;

  G4VPhysicalVolume *prev = pre_step_point->GetPhysicalVolume();
  G4String volume = prev->GetName();
  const char *name = volume.c_str();

  m_number = int(name[strlen(name) - 1]) - 0x30;
  m_x = p.getX();
  m_y = p.getY();
  if(fabs(m_x) > 50.*CLHEP::mm || fabs(m_y) > 50.*CLHEP::mm){
    G4cout << "BeamCh(" << m_number
           << ") hit is out of 100x100 region" << G4endl;
    return false;
  }

  //G4cout << "Identifier(" << m_number << ") -> ("
  //       << m_x << ", " << m_y << ")" << G4endl;

  return true;
}
