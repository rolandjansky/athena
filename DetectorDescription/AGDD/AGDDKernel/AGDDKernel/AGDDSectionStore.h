/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSectionStore_H
#define AGDDSectionStore_H

class AGDDSection;

#include <map>
#include <string>

typedef std::map<std::string,AGDDSection*> sectionList;

class AGDDSectionStore {
public:
	AGDDSectionStore();
	AGDDSection* GetSection(const std::string&);
	AGDDSection* GetCurrentSection() {return m_currentSection;}
	void RegisterSection(AGDDSection*);
	void PrintAllSections() const;
	void Clean();
private:
	AGDDSection* m_currentSection;
	sectionList m_theSections;
};

#endif
