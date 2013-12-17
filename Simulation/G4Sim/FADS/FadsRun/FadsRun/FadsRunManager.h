/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FadsRunManager_H
#define FadsRunManager_H

#include "G4RunManager.hh"

#include <iostream>

#include "FadsPhysics/FadsPhysicsList.h"

namespace FADS {

class FadsRunManager: public G4RunManager {
private:
	FadsPhysicsList *pl;
protected:
	void InitializeGeometry();
	void InitializePhysics();
public:
	FadsRunManager();
	~FadsRunManager();
	void SetPhysicsList(FadsPhysicsList *p);
	G4Event* GenerateEvent(G4int i_event);
};

}	// end namespace
                
#endif
