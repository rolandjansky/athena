/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDSection_H
#define AGDDSection_H

class AGDDVolume;
class AGDDSectionStore;

#include <string>
#include <map>

typedef std::map<std::string, AGDDVolume*> volumeList;
typedef volumeList::const_iterator volumeIterator;

class AGDDSection {
public:
	AGDDSection(const std::string&, const std::string&, const std::string&,
                    const std::string&, const std::string&,
                    AGDDSectionStore& ss,
                    bool buildFlag=true);
	void AddVolume(AGDDVolume*);
	AGDDVolume* GetVolume(const std::string&);
	void Register(AGDDSectionStore& ss);
	void Name(const std::string& n) {m_name=n;}
	void Version(const std::string& n) {m_version=n;}
	void Author(const std::string& n) {m_author=n;}
	void Date(const std::string& n) {m_date=n;}
	void TopVolume(const std::string& n) {m_topVolume=n;}
	std::string Name() const {return m_name;}
	std::string Version() const {return m_version;}
	std::string Author() const {return m_author;}
	std::string Date() const {return m_date;}
	std::string TopVolume() const {return m_topVolume;}
	bool IsToBeBuilt() const {return m_toBeBuilt;}
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
