/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ActionStoreMessenger_H
#define ActionStoreMessenger_H

#include "G4UImessenger.hh"
 
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIdirectory;

namespace FADS {

class ActionsStore;

class ActionStoreMessenger: public G4UImessenger {
private:
    ActionsStore *rm;
	G4UIdirectory *directory;
    G4UIcommand *list;
	G4UIcommand *select;
	G4UIcommand *del;
	G4UIcommand *reg;
public:
    ActionStoreMessenger(ActionsStore *v);
    ~ActionStoreMessenger();
    void SetNewValue(G4UIcommand * command,G4String newValues);
    G4String GetCurrentValue(G4UIcommand * command);
};

}	// end namespace

#endif
