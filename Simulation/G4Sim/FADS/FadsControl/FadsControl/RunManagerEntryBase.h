/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RunManagerEntryBase_H
#define RunManagerEntryBase_H

#include <string>
#include "FadsControl/RunManagerStore.h"
class G4RunManager;

class RunManagerEntryBase {
public:
	RunManagerEntryBase(std::string n):name_(n) 
	{
		RunManagerStore::AddRunManagerEntry(this);
	}
	virtual ~RunManagerEntryBase() {}
	std::string name() {return name_;}
	virtual void CreateEntry() =0;
	virtual void DeleteEntry() =0;
	virtual G4RunManager* RunManager() = 0;
protected:
	std::string name_;
};

#endif
