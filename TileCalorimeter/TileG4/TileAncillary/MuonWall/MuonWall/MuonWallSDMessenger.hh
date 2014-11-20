/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MuonWallSDMessenger.
// Messenger for TileTB SensitiveDetector class.
//
// Author: F. Martin
// May 5, 2004
//
//************************************************************


#ifndef MuonWallSDMessenger_H
#define MuonWallSDMessenger_H

#include "G4UImessenger.hh"

class MuonWallSD;
class G4UIcmdWithAString;


class MuonWallSDMessenger: public G4UImessenger
{
public:
  MuonWallSDMessenger(MuonWallSD*);
  ~MuonWallSDMessenger();

  void SetNewValue(G4UIcommand*, G4String);
 
private:
  MuonWallSD* sensitiveDetector;
  G4UIcmdWithAString* descriptionCmd;   
};

#endif
