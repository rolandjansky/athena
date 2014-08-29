/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPLArVolumeAccept_H
#define HIPLArVolumeAccept_H

#include "FadsActions/UserAction.h"
#include <string>

class HIPLArVolumeAccept: public FADS::UserAction {

public:
	HIPLArVolumeAccept(std::string s): UserAction(s), HIPacc(0), HIPevts(0), HIPevts_failed(0) {}
	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);

private:
        bool HIPacc;
        int HIPevts;
        int HIPevts_failed;

};


#endif
