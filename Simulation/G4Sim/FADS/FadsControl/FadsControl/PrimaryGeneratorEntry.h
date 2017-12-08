/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryGeneratorEntry_H
#define PrimaryGeneratorEntry_H

#include "FadsControl/PrimaryGeneratorEntryBase.h"
#include "G4VUserPrimaryGeneratorAction.hh"

template<class T> 
class PrimaryGeneratorEntry:public PrimaryGeneratorEntryBase {
public:
	PrimaryGeneratorEntry(std::string n):PrimaryGeneratorEntryBase(n),
				        thePrimaryGenerator(0) {}
	void CreateEntry() {thePrimaryGenerator=new T;}
	void DeleteEntry() {
		if (thePrimaryGenerator) 
		{delete thePrimaryGenerator;thePrimaryGenerator=0;}
	}
	G4VUserPrimaryGeneratorAction* PrimaryGenerator() {return thePrimaryGenerator;} 
private:
	G4VUserPrimaryGeneratorAction* thePrimaryGenerator;
};
#endif
