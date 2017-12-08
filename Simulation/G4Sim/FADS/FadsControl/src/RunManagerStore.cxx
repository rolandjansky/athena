/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsControl/RunManagerStore.h"
#include "FadsControl/RunManagerEntryBase.h"

#include "G4RunManager.hh"

runManagerMap RunManagerStore::theRunManagers=runManagerMap();
std::string  RunManagerStore::selectedRunManager="";

RunManagerStore::RunManagerStore()
{
}

void RunManagerStore::AddRunManagerEntry(RunManagerEntryBase *pl)
{
	std::string tempName=pl->name();
	std::cout<<"trying to add run manager "<<tempName<<" to catalog"<<std::endl;
	if (theRunManagers.find(tempName)!=theRunManagers.end())
		std::cout<<" Run manager entry "<<tempName<<" already exists!"
			 <<std::endl;
	else
	{
		theRunManagers[tempName]=pl;
	}
	std::cout<<"RunManagerStore done"<<std::endl;
}
void RunManagerStore::SelectRunManager(std::string s)
{
	std::cout<<" RunManagerStore::SelectRunManager "<<s<<std::endl;
	if (theRunManagers.find(s)!=theRunManagers.end())
	{
		theRunManagers[s]->CreateEntry();
		if (!selectedRunManager.empty())
			theRunManagers[selectedRunManager]->DeleteEntry();
		selectedRunManager=s;
	}
	else
		std::cout<<" entry "<<s<<" not found!"<<std::endl;	
}
void RunManagerStore::UnselectRunManager(std::string s)
{
	if (s==selectedRunManager)
	{
		theRunManagers[selectedRunManager]->DeleteEntry();
		selectedRunManager="";
	}
}

void RunManagerStore::PrintRunManagers()
{
	std::cout<<" -- Catalog of all available run managers --"<<std::endl;
 	runManagerMap::const_iterator it;
 	for (it=theRunManagers.begin();it!=theRunManagers.end();it++)
 	{
		std::cout<<"\t\t "<<(*it).first<<std::endl;
 	}
	std::cout<<" --"<<std::endl;
}

void RunManagerStore::SelectedRunManager()
{
	std::cout<<" Currently selected RunManager: "<<selectedRunManager<<std::endl;
}
