/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RunManagerEntry_H
#define RunManagerEntry_H

#include "FadsControl/RunManagerEntryBase.h"
#include "G4VUserDetectorConstruction.hh"

template<class T> 
class RunManagerEntry:public RunManagerEntryBase {
public:
	RunManagerEntry(std::string n):RunManagerEntryBase(n),
				        theRunManager(0) {}
	void CreateEntry() {theRunManager=new T;}
	void DeleteEntry() {
		if (theRunManager) 
		{delete theRunManager;theRunManager=0;}
	}
	G4RunManager* RunManager() {return theRunManager;} 
private:
	G4RunManager* theRunManager;
};
#endif
