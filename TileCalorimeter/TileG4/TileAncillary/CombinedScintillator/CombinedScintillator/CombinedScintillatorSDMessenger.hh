/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class CombinedScintillatorSDMessenger.
// Messenger for TileTB SensitiveDetector class.
//
// Author: V. Tsulaia
// August 22, 2002.
//
//************************************************************


#ifndef CombinedScintillatorSDMessenger_H
#define CombinedScintillatorSDMessenger_H

#include "G4UImessenger.hh"

class CombinedScintillatorSD;
class G4UIcmdWithAString;


class CombinedScintillatorSDMessenger: public G4UImessenger
{
public:
  CombinedScintillatorSDMessenger(CombinedScintillatorSD*);
  ~CombinedScintillatorSDMessenger();

  void SetNewValue(G4UIcommand*, G4String);
 
private:
  CombinedScintillatorSD* sensitiveDetector;
  G4UIcmdWithAString* descriptionCmd;   
};

#endif
