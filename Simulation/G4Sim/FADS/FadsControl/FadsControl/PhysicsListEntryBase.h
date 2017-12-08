/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListEntryBase_H
#define PhysicsListEntryBase_H

#include <string>
#include "FadsControl/PhysicsListManager.h"
class G4VUserPhysicsList;

class PhysicsListEntryBase {
public:
	PhysicsListEntryBase(std::string n):name_(n) 
	{
		PhysicsListManager::AddPhysicsListEntry(this);
	}
	virtual ~PhysicsListEntryBase() {}
	std::string name() {return name_;}
	virtual void CreateEntry() =0;
	virtual void DeleteEntry() =0;
	virtual G4VUserPhysicsList* PhysicsList() = 0;
protected:
	std::string name_;
};

#endif
