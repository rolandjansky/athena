/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
	AGDDController();
	~AGDDController();
	void SetBuilder(AGDDBuilder *b);
	void SetParser(IAGDDParser *b);
	AGDDBuilder* GetBuilder();
	IAGDDParser* GetParser();
	void AddFile(const std::string& fName);
	void AddSection(const std::string& section);
	void AddVolume(const std::string& volume);
	void ParseFiles();
	void ParseString(const std::string&);
	bool WriteAGDDtoDBFile(const std::string&);
	void PrintSections() const;
	void BuildVolumes();
	void BuildSections();
	void BuildAll();
	void Clean();
	
	static void PrintVolumeHierarchy(const std::string&, int);
	
	void UseGeoModelDetector(const std::string&);
	
	void Locked(bool b) {m_locked=b;}
	bool Locked() const {return m_locked;}
	
	void DisableSections(bool b) {m_disableSections=b;}
	bool DisableSections() const {return m_disableSections;}
private:
	
        AGDDController(const AGDDController& c) = delete;
        AGDDController& operator=(const AGDDController& c) = delete;
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
