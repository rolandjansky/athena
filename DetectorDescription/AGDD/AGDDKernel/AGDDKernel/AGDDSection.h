/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSection_H
#define AGDDSection_H

class AGDDVolume;

#include <string>
#include <map>

typedef std::map<std::string, AGDDVolume*> volumeList;
typedef volumeList::const_iterator volumeIterator;

class AGDDSection {
public:
	AGDDSection(std::string, std::string, std::string, std::string, std::string, bool buildFlag=true);
	void AddVolume(AGDDVolume*);
	AGDDVolume* GetVolume(std::string);
	void Register();
	void Name(std::string n) {m_name=n;}
	void Version(std::string n) {m_version=n;}
	void Author(std::string n) {m_author=n;}
	void Date(std::string n) {m_date=n;}
	void TopVolume(std::string n) {m_topVolume=n;}
	std::string Name() {return m_name;}
	std::string Version() {return m_version;}
	std::string Author() {return m_author;}
	std::string Date() {return m_date;}
	std::string TopVolume() {return m_topVolume;}
	bool IsToBeBuilt() {return m_toBeBuilt;}
	volumeIterator VolumeBegin() {return m_theVolumes.begin();}
	volumeIterator VolumeEnd() {return m_theVolumes.end();}
	void Print();
private:
	std::string m_name;
	std::string m_version;
	std::string m_author;
	std::string m_date;
	std::string m_topVolume;
        bool m_toBeBuilt;

	volumeList m_theVolumes;
};

#endif
