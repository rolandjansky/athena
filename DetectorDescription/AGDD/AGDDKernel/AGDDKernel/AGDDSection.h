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
	void Name(std::string n) {name=n;}
	void Version(std::string n) {version=n;}
	void Author(std::string n) {author=n;}
	void Date(std::string n) {date=n;}
	void TopVolume(std::string n) {topVolume=n;}
	std::string Name() {return name;}
	std::string Version() {return version;}
	std::string Author() {return author;}
	std::string Date() {return date;}
	std::string TopVolume() {return topVolume;}
	bool IsToBeBuilt() {return toBeBuilt;}
	volumeIterator VolumeBegin() {return theVolumes.begin();}
	volumeIterator VolumeEnd() {return theVolumes.end();}
	void Print();
private:
	std::string name;
	std::string version;
	std::string author;
	std::string date;
	std::string topVolume;
        bool toBeBuilt;

	volumeList theVolumes;
};

#endif
