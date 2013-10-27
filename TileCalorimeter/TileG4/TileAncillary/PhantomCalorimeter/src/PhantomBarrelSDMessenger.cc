/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class PhantomBarrelSDMessenger.
// Messenger for PhantomBarrel SensitiveDetector class.
//
// Author: F. Martin
// May 5, 2004.
//
//************************************************************

#include "PhantomCalorimeter/PhantomBarrelSDMessenger.hh"
#include "PhantomCalorimeter/PhantomBarrelSD.hh"
#include "G4UIcmdWithAString.hh"

PhantomBarrelSDMessenger::PhantomBarrelSDMessenger(PhantomBarrelSD* sDetector)
    : sensitiveDetector(sDetector)
{
  descriptionCmd = new G4UIcmdWithAString("/PhantomBarrelSD/GetDescription",this);
  descriptionCmd->SetGuidance("Obtains Detector Description for PhantomBarrel Sensitive Detector");
  descriptionCmd->SetParameterName("choice",true);

}

PhantomBarrelSDMessenger::~PhantomBarrelSDMessenger()
{
  delete descriptionCmd;
}

void PhantomBarrelSDMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == descriptionCmd)
  {
    sensitiveDetector->Description(newValue);
  }

}
