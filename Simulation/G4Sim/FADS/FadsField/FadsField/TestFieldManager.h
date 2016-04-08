/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TestFieldManager_H
#define TestFieldManager_H

#include "FadsField/FadsFieldManager.h"

class TestFieldManager: public FADS::FadsFieldManager {
public:
	TestFieldManager(std::string s):FADS::FadsFieldManager(s) {;}
	void ConfigureForTrack(const G4Track* );
};

#endif
