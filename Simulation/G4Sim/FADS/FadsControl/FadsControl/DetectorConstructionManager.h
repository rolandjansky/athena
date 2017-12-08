/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorConstructionManager_H
#define DetectorConstructionManager_H

#include <iostream>
#include <string>
#include <map>

class G4VUserDetectorConstruction;
class DetectorConstructionEntryBase;

typedef std::map<std::string, DetectorConstructionEntryBase*, std::less<std::string> > detConstructionMap;

class DetectorConstructionManager {
public:
	DetectorConstructionManager();
	
	static void AddDetectorConstructionEntry(DetectorConstructionEntryBase *);
	static void SelectDetectorConstruction(std::string);
	static void UnselectDetectorConstruction(std::string);
	static void PrintDetectorConstructions();
	static void SelectedDetectorConstruction();
private:	
	static detConstructionMap theDetectors;
	static std::string selectedDetector;
};

struct DetectorConstructionManagerFacade
{ 				
	void SelectDetectorConstruction(std::string s)
		{DetectorConstructionManager::SelectDetectorConstruction(s);}
	void UnselectDetectorConstruction(std::string s)
		{DetectorConstructionManager::UnselectDetectorConstruction(s);}
	void PrintDetectorConstructions()
		{DetectorConstructionManager::PrintDetectorConstructions();}
	void SelectedDetectorConstruction()
		{DetectorConstructionManager::SelectedDetectorConstruction();}
};

#endif
