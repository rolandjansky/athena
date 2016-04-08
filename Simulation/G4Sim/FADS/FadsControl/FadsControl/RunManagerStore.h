/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RunManagerStore_H
#define RunManagerStore_H

#include <iostream>
#include <string>
#include <map>

class G4RunManager;
class RunManagerEntryBase;

typedef std::map<std::string, RunManagerEntryBase*, std::less<std::string> > runManagerMap;

class RunManagerStore {
public:
	RunManagerStore();
	
	static void AddRunManagerEntry(RunManagerEntryBase *);
	static void SelectRunManager(std::string);
	static void UnselectRunManager(std::string);
	static void PrintRunManagers();
	static void SelectedRunManager();
private:	
	static runManagerMap theRunManagers;
	static std::string selectedRunManager;
	static RunManagerStore* theRunManagerStore;
};

struct RunManagerStoreFacade
{ 				
	void SelectRunManager(std::string s)
		{ RunManagerStore::SelectRunManager(s);}
	void UnselectRunManager(std::string s)
		{RunManagerStore::UnselectRunManager(s);}
	void PrintRunManagers()
		{RunManagerStore::PrintRunManagers();}
	void SelectedRunManager()
		{RunManagerStore::SelectedRunManager();}
};

#endif
