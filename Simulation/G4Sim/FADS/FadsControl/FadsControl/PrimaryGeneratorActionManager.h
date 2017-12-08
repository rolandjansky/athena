/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryGeneratorActionManager_H
#define PrimaryGeneratorActionManager_H

#include <iostream>
#include <string>
#include <map>

class G4VUserPrimaryGeneratorAction;
class PrimaryGeneratorEntryBase;

typedef std::map<std::string, PrimaryGeneratorEntryBase*, std::less<std::string> > pGeneratorMap;

class PrimaryGeneratorActionManager {
public:
	PrimaryGeneratorActionManager();
	static void AddPrimaryGeneratorEntry(PrimaryGeneratorEntryBase *);
	static void SelectPrimaryGenerator(std::string);
	static void UnselectPrimaryGenerator(std::string);
	static void PrintPrimaryGenerators();
	static void SelectedPrimaryGenerator();
	
private:
	static pGeneratorMap theGenerators;
	static std::string selectedGenerator;

};

struct PrimaryGeneratorActionManagerFacade {
	void SelectPrimaryGenerator(std::string s)
		{PrimaryGeneratorActionManager::SelectPrimaryGenerator(s);}
	void UnselectPrimaryGenerator(std::string s)
		{PrimaryGeneratorActionManager::UnselectPrimaryGenerator(s);}
	void PrintPrimaryGenerators()
		{PrimaryGeneratorActionManager::PrintPrimaryGenerators();}
	void SelectedPrimaryGenerator()
		{PrimaryGeneratorActionManager::SelectedPrimaryGenerator();}
				
};

#endif
