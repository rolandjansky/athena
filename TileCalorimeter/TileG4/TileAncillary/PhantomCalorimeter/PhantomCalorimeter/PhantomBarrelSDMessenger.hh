/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class PhantomBarrelSDMessenger.
// Messenger for TileTB SensitiveDetector class.
//
// Author: V. Tsulaia
// August 22, 2002.
//
//************************************************************


#ifndef PhantomBarrelSDMessenger_H
#define PhantomBarrelSDMessenger_H

#include "G4UImessenger.hh"

class PhantomBarrelSD;
class G4UIcmdWithAString;


class PhantomBarrelSDMessenger: public G4UImessenger
{
public:
  PhantomBarrelSDMessenger(PhantomBarrelSD*);
  ~PhantomBarrelSDMessenger();

  void SetNewValue(G4UIcommand*, G4String);
 
private:
  PhantomBarrelSD* sensitiveDetector;
  G4UIcmdWithAString* descriptionCmd;   
};

#endif
