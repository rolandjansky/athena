/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillatorSDMessenger.
// Messenger for CombinedScintillator SensitiveDetector class.
//
// Author: F. Martin
// May 5, 2004.
//
//************************************************************

#include "CombinedScintillator/CombinedScintillatorSDMessenger.hh"
#include "CombinedScintillator/CombinedScintillatorSD.hh"
#include "G4UIcmdWithAString.hh"

CombinedScintillatorSDMessenger::CombinedScintillatorSDMessenger(CombinedScintillatorSD* sDetector)
    : sensitiveDetector(sDetector)
{
  descriptionCmd = new G4UIcmdWithAString("/CombinedScintillatorSD/GetDescription",this);
  descriptionCmd->SetGuidance("Obtains Detector Description for CombinedScintillator Sensitive Detector");
  descriptionCmd->SetParameterName("choice",true);

}

CombinedScintillatorSDMessenger::~CombinedScintillatorSDMessenger()
{
  delete descriptionCmd;
}

void CombinedScintillatorSDMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == descriptionCmd)
  {
    sensitiveDetector->Description(newValue);
  }

}
