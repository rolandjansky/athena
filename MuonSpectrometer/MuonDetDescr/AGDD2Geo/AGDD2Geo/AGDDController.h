/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDController_H
#define AGDDController_H

class AGDDBuilder;
class IAGDDParser;
class GeoFullPhysVol;

#include "AGDD2Geo/AGDDSectionStore.h"

#include <string>
#include <vector>

class AGDDController {
public:
	static AGDDController* GetController();
	void SetBuilder(AGDDBuilder *b);
	void SetParser(IAGDDParser *b);
	AGDDBuilder* GetBuilder();
	IAGDDParser* GetParser();
	void AddFile(std::string fName);
	void AddSection(std::string section);
	void AddVolume(std::string volume);
	void AddStructure(std::string volume);
	void ParseFiles();
	void ParseString(std::string);
	void ParseAMDC();
	void PrintSections() const;
	void BuildVolumes();
	void BuildVolumesFromFlags();
	void BuildSections();
	void BuildStructures();
	void BuildAll();
	std::map<std::string, GeoFullPhysVol*>* GetMSdetectors() const;
	//	  {
	//	    if (theBuilder) return theBuilder->getMSdetectors();
	//	    else return 0;
	//	  }
	
	static void PrintVolumeHierarchy(std::string, int);
	
	void UseGeoModelDetector(std::string);
	void NavigateGeoModelDetector(std::string);
private:

	std::string GetAMDC();
	
	AGDDController();
	IAGDDParser *theParser;
	AGDDBuilder* theBuilder;
	
	std::vector<std::string> filesToParse;
	std::vector<std::string> sectionsToBuild;
	std::vector<std::string> volumesToBuild;
	std::vector<std::string> structuresToBuild;
	
	int printLevel;
};

#endif
