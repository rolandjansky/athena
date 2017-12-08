/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsControl/DetectorConstructionManager.h"
#include "FadsControl/DetectorConstructionEntryBase.h"
#include "G4VUserDetectorConstruction.hh"

#include "G4RunManager.hh"

detConstructionMap DetectorConstructionManager::theDetectors=detConstructionMap();
std::string  DetectorConstructionManager::selectedDetector="";

DetectorConstructionManager::DetectorConstructionManager()
{
}

void DetectorConstructionManager::AddDetectorConstructionEntry(DetectorConstructionEntryBase *pl)
{
	std::string tempName=pl->name();
	std::cout<<"trying to add detector construction "<<tempName<<" to catalog"<<std::endl;
	if (theDetectors.find(tempName)!=theDetectors.end())
		std::cout<<" Detector construction entry "<<tempName<<" already exists!"
			 <<std::endl;
	else
	{
		theDetectors[tempName]=pl;
	}
	std::cout<<"DetectorConstructionManager done"<<std::endl;
}
void DetectorConstructionManager::SelectDetectorConstruction(std::string s)
{
	if (theDetectors.find(s)!=theDetectors.end())
	{
		G4RunManager *rm=G4RunManager::GetRunManager();
		if (!rm) rm=new G4RunManager;
		theDetectors[s]->CreateEntry();
		rm->SetUserInitialization(theDetectors[s]->DetectorConstruction());
		if (!selectedDetector.empty())
			theDetectors[selectedDetector]->DeleteEntry();
		selectedDetector=s;
	}
	else
		std::cout<<" entry "<<s<<" not found!"<<std::endl;	
}
void DetectorConstructionManager::UnselectDetectorConstruction(std::string s)
{
	if (s==selectedDetector)
	{
		theDetectors[selectedDetector]->DeleteEntry();
		selectedDetector="";
	}
}

void DetectorConstructionManager::PrintDetectorConstructions()
{
	std::cout<<" -- Catalog of all available detector constructions --"<<std::endl;
 	detConstructionMap::const_iterator it;
 	for (it=theDetectors.begin();it!=theDetectors.end();it++)
 	{
		std::cout<<"\t\t "<<(*it).first<<std::endl;
 	}
	std::cout<<" --"<<std::endl;
}

void DetectorConstructionManager::SelectedDetectorConstruction()
{
	std::cout<<" Currently selected DetectorConstruction: "<<selectedDetector<<std::endl;
}
