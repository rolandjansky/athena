/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorDescriptionStore_H
#define DetectorDescriptionStore_H

#include <string>
#include <map>

namespace FADS {

class DetectorDescription;
//class DescriptionMessenger;

typedef std::map<std::string, DetectorDescription*, std::less<std::string> > DetDescMap;
typedef std::map<std::string,DetDescMap*,std::less<std::string> > NameSpaces;
typedef DetDescMap::const_iterator DetDescIterator;

class DetectorDescriptionStore {
private:
	DetectorDescriptionStore();
	DetDescMap* theMap;
	NameSpaces theNameSpaces;
//	DescriptionMessenger *theMessenger;
	std::string oldNameSpace;
	std::string currentNameSpace;
public:
	void UseNameSpace(std::string s);
	void ResetNameSpace();
	bool FindDetectorDescription(std::string s);
	static DetectorDescriptionStore* GetDetectorDescriptionStore();
	void AddDetectorDescription(DetectorDescription *d);
	void RemoveDetectorDescription(std::string nam);
	void ReplaceDetectorDescription(std::string nam,const DetectorDescription *d);
	void PrintDetectorDescription(std::string nam);
	DetectorDescription* GetDetectorDescription(std::string name);
	DetDescIterator DetDescBegin();
	DetDescIterator DetDescEnd();
	void PrintAll();
};

}	// end namespace

#endif
