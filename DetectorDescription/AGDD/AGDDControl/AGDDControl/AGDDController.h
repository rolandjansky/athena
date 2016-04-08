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
	
	void Locked(bool b) {m_locked=b;}
	bool Locked() {return m_locked;}
	
	void DisableSections(bool b) {m_disableSections=b;}
	bool DisableSections() {return m_disableSections;}
private:
	
	AGDDController();
	AGDDController(const AGDDController& c):m_theParser(c.m_theParser),m_theBuilder(c.m_theBuilder) {;}
	AGDDController& operator=(const AGDDController& c) 
	{
		if (this!=&c) {
			m_theParser=c.m_theParser;
			m_theBuilder=c.m_theBuilder;
		}
		return *this;
	}
	IAGDDParser *m_theParser;
	AGDDBuilder* m_theBuilder;
	
	std::vector<std::string> m_filesToParse;
	std::vector<std::string> m_sectionsToBuild;
	std::vector<std::string> m_volumesToBuild;
	std::vector<std::string> m_structuresToBuild;
	
	bool m_locked;
	bool m_disableSections;
	
	int m_printLevel;
};

#endif
