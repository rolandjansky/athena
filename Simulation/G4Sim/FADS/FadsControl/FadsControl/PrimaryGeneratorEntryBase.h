/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryGeneratorEntryBase_H
#define PrimaryGeneratorEntryBase_H

#include <string>
#include "FadsControl/PrimaryGeneratorActionManager.h"
class G4VUserPrimaryGeneratorAction;

class PrimaryGeneratorEntryBase {
public:
	PrimaryGeneratorEntryBase(std::string n):name_(n) 
	{
		PrimaryGeneratorActionManager::AddPrimaryGeneratorEntry(this);
	}
	virtual ~PrimaryGeneratorEntryBase() {}
	std::string name() {return name_;}
	virtual void CreateEntry() =0;
	virtual void DeleteEntry() =0;
	virtual G4VUserPrimaryGeneratorAction* PrimaryGenerator() = 0;
protected:
	std::string name_;
};

#endif
