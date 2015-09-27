/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDController_H
#define AGDDController_H

class AGDDBuilder;
class IAGDDParser;
class GeoFullPhysVol;

#include "AGDDKernel/AGDDSectionStore.h"

#include <string>
#include <vector>

class AGDDController {
public:
	~AGDDController();
	static AGDDController* GetController();
	void SetBuilder(AGDDBuilder *b);
	void SetParser(IAGDDParser *b);
	AGDDBuilder* GetBuilder();
	IAGDDParser* GetParser();
	void AddFile(std::string fName);
	void AddSection(std::string section);
	void AddVolume(std::string volume);
	void ParseFiles();
	void ParseString(std::string);
	void PrintSections() const;
	void BuildVolumes();
	void BuildSections();
	void BuildAll();
	void Clean();
	
	static void PrintVolumeHierarchy(std::string, int);
	
	void UseGeoModelDetector(std::string);
	
	void Locked(bool b) {locked=b;}
	bool Locked() {return locked;}
	
	void DisableSections(bool b) {disableSections=b;}
	bool DisableSections() {return disableSections;}
private:
	
	AGDDController();
	AGDDController(const AGDDController& c):theParser(c.theParser),theBuilder(c.theBuilder) {;}
	AGDDController& operator=(const AGDDController& c) 
	{
		if (this!=&c) {
			theParser=c.theParser;
			theBuilder=c.theBuilder;
		}
		return *this;
	}
	IAGDDParser *theParser;
	AGDDBuilder* theBuilder;
	
	std::vector<std::string> filesToParse;
	std::vector<std::string> sectionsToBuild;
	std::vector<std::string> volumesToBuild;
	std::vector<std::string> structuresToBuild;
	
	bool locked;
	bool disableSections;
	
	int printLevel;
};

#endif
