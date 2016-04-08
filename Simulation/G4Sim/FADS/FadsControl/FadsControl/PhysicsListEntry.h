/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListEntry_H
#define PhysicsListEntry_H

#include "FadsControl/PhysicsListEntryBase.h"
#include "G4VUserPhysicsList.hh"

template<class T> 
class PhysicsListEntry:public PhysicsListEntryBase {
public:
	PhysicsListEntry(std::string n):PhysicsListEntryBase(n),
				        thePhysicsList(0) {}
	void CreateEntry() {thePhysicsList=new T;}
	void DeleteEntry() {
		if (thePhysicsList) 
		{delete thePhysicsList;thePhysicsList=0;}
	}
	G4VUserPhysicsList* PhysicsList() {return thePhysicsList;} 
private:
	G4VUserPhysicsList* thePhysicsList;
};
#endif
