/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSectionStore_H
#define AGDDSectionStore_H

class AGDDSection;

#include <map>
#include <string>

typedef std::map<std::string,AGDDSection*> sectionList;

class AGDDSectionStore {
public:
	static AGDDSectionStore* GetSectionStore();
	AGDDSection* GetSection(std::string);
	AGDDSection* GetCurrentSection() {return m_currentSection;}
	void RegisterSection(AGDDSection*);
	void PrintAllSections();
	void Clean();
private:
	AGDDSectionStore();
	AGDDSection* m_currentSection;
	sectionList m_theSections;
};

#endif
