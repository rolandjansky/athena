/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PhysicsListManager_H
#define PhysicsListManager_H

#include <iostream>
#include <string>
#include <map>

class G4VUserPhysicsList;
class PhysicsListEntryBase;

typedef std::map<std::string, PhysicsListEntryBase*, std::less<std::string> > physListMap;

class PhysicsListManager {
public:
	PhysicsListManager();
	static void AddPhysicsListEntry(PhysicsListEntryBase *);
	static void SelectPhysicsList(std::string);
	static void UnselectPhysicsList(std::string);
	static void PrintPhysicsLists();
	static void SelectedPhysicsList();
	
private:
	static physListMap theLists;
	static std::string selectedList;
	
};

struct PhysicsListManagerFacade {
	void SelectPhysicsList(std::string s)
		{PhysicsListManager::SelectPhysicsList(s);}
	void UnselectPhysicsList(std::string s)
		{PhysicsListManager::UnselectPhysicsList(s);}
	void PrintPhysicsLists()
		{PhysicsListManager::PrintPhysicsLists();}
	void SelectedPhysicsList()
		{PhysicsListManager::SelectedPhysicsList();}
				
};

#endif
