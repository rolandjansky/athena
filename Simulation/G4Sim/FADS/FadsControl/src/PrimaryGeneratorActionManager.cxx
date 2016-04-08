/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsControl/PrimaryGeneratorActionManager.h"
#include "FadsControl/PrimaryGeneratorEntryBase.h"

#include "G4RunManager.hh"

pGeneratorMap PrimaryGeneratorActionManager::theGenerators=pGeneratorMap();
std::string PrimaryGeneratorActionManager::selectedGenerator="";

PrimaryGeneratorActionManager::PrimaryGeneratorActionManager()
{
}
void PrimaryGeneratorActionManager::AddPrimaryGeneratorEntry(PrimaryGeneratorEntryBase *pl)
{
	std::string tempName=pl->name();
	std::cout<<"trying to add primary generator "<<tempName<<" to catalog"<<std::endl;
	if (theGenerators.find(tempName)!=theGenerators.end())
		std::cout<<" generator action entry "<<tempName<<" already exists!"
			 <<std::endl;
	else
		theGenerators[tempName]=pl;
	std::cout<<"PrimaryGeneratorActionManager done"<<std::endl;
}
void PrimaryGeneratorActionManager::SelectPrimaryGenerator(std::string s)
{
	if (theGenerators.find(s)!=theGenerators.end())
	{
		G4RunManager *rm=G4RunManager::GetRunManager();
		if (!rm) rm=new G4RunManager;
		theGenerators[s]->CreateEntry();
		rm->SetUserAction(theGenerators[s]->PrimaryGenerator());
		if (!selectedGenerator.empty())
			theGenerators[selectedGenerator]->DeleteEntry();
		selectedGenerator=s;
	}
	else
		std::cout<<" entry "<<s<<" not found!"<<std::endl;	
}
void PrimaryGeneratorActionManager::UnselectPrimaryGenerator(std::string s)
{
	if (s==selectedGenerator)
	{
		theGenerators[selectedGenerator]->DeleteEntry();
		selectedGenerator="";
	}
}

void PrimaryGeneratorActionManager::PrintPrimaryGenerators()
{
	std::cout<<" -- Catalog of all available primary generators --"<<std::endl;
 	pGeneratorMap::const_iterator it;
 	for (it=theGenerators.begin();it!=theGenerators.end();it++)
 	{
		std::cout<<"\t\t "<<(*it).first<<std::endl;
 	}
	std::cout<<" --"<<std::endl;
}

void PrimaryGeneratorActionManager::SelectedPrimaryGenerator()
{
	std::cout<<" Currently selected PrimaryGenerator: "<<selectedGenerator<<std::endl;
}
