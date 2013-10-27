/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWallSDMessenger.
// Messenger for MuonWall SensitiveDetector class.
//
// Author: F. Martin
// May 5, 2004.
//
//************************************************************

#include "MuonWall/MuonWallSDMessenger.hh"
#include "MuonWall/MuonWallSD.hh"
#include "G4UIcmdWithAString.hh"

MuonWallSDMessenger::MuonWallSDMessenger(MuonWallSD* sDetector)
    : sensitiveDetector(sDetector)
{
  descriptionCmd = new G4UIcmdWithAString("/MuonWallSD/GetDescription",this);
  descriptionCmd->SetGuidance("Obtains Detector Description for MuonWall Sensitive Detector");
  descriptionCmd->SetParameterName("choice",true);

}

MuonWallSDMessenger::~MuonWallSDMessenger()
{
  delete descriptionCmd;
}

void MuonWallSDMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == descriptionCmd)
  {
    sensitiveDetector->Description(newValue);
  }

}
