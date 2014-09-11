/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsRunManager_H
#define FadsRunManager_H

#include "G4RunManager.hh"

class FadsRunManager: public G4RunManager {
public:
	virtual ~FadsRunManager() {;}
	virtual void InitializeRun();
	virtual void TerminateRun();
	virtual void SimulateOneEvent();
};

#endif
