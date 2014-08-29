/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhotonKiller_H
#define PhotonKiller_H

#include "FadsActions/UserAction.h"
#include <string>

class PhotonKiller: public FADS::UserAction {

public:
	PhotonKiller(std::string s): UserAction(s) { }
	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);

private:

};


#endif
