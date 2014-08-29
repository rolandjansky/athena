/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VolumeDebugger_H
#define VolumeDebugger_H

#include "FadsActions/UserAction.h"
#include <string>

class G4LogicalVolume;

class VolumeDebugger: public FADS::UserAction {

public:
	VolumeDebugger(std::string s): UserAction(s),m_init(false) {}
    void EndOfEventAction(const G4Event*);
private:
	bool m_init;
    void PullVolumes( G4LogicalVolume* v ) const;
};

#endif
