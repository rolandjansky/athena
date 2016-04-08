/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsControl/PhysicsListManager.h"
#include "FadsControl/PhysicsListEntryBase.h"
#include "G4VUserPhysicsList.hh"

#include "G4RunManager.hh"

physListMap PhysicsListManager::theLists=physListMap();
std::string PhysicsListManager::selectedList="";

PhysicsListManager::PhysicsListManager()
{
}
void PhysicsListManager::AddPhysicsListEntry(PhysicsListEntryBase *pl)
{
	std::string tempName=pl->name();
	std::cout<<"trying to add physics list "<<tempName<<" to catalog"<<std::endl;
	if (theLists.find(tempName)!=theLists.end())
		std::cout<<" Physics list entry "<<tempName<<" already exists!"
			 <<std::endl;
	else
		theLists[tempName]=pl;
	std::cout<<"PhysicsListManager done"<<std::endl;
}
void PhysicsListManager::SelectPhysicsList(std::string s)
{
	if (theLists.find(s)!=theLists.end())
	{
		G4RunManager *rm=G4RunManager::GetRunManager();
		if (!rm) rm=new G4RunManager;
		theLists[s]->CreateEntry();
		rm->SetUserInitialization(theLists[s]->PhysicsList());
		if (!selectedList.empty())
			theLists[selectedList]->DeleteEntry();
		selectedList=s;
	}
	else
		std::cout<<" entry "<<s<<" not found!"<<std::endl;	
}
void PhysicsListManager::UnselectPhysicsList(std::string s)
{
	if (s==selectedList)
	{
		theLists[selectedList]->DeleteEntry();
		selectedList="";
	}
}

void PhysicsListManager::PrintPhysicsLists()
{
	std::cout<<" -- Catalog of all available physics lists --"<<std::endl;
 	physListMap::const_iterator it;
 	for (it=theLists.begin();it!=theLists.end();it++)
 	{
		std::cout<<"\t\t "<<(*it).first<<std::endl;
 	}
	std::cout<<" --"<<std::endl;
}

void PhysicsListManager::SelectedPhysicsList()
{
	std::cout<<" Currently selected PhysicsList: "<<selectedList<<std::endl;
}
