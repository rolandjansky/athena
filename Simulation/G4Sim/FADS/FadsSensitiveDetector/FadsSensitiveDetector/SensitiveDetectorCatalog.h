/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SensitiveDetectorCatalog_H
#define SensitiveDetectorCatalog_H

#include <map>
#include <string>
#include "FadsSensitiveDetector/SensitiveDetectorEntry.h"
#include "FadsSensitiveDetector/FadsSensitiveDetector.h"

namespace FADS {

//class SensitiveDetectorMessenger;

typedef std::map<std::string,SensitiveDetectorEntry *,std::less<std::string> > SDCatalog;
typedef std::map<std::string,FadsSensitiveDetector *,std::less<std::string> >  SDStore;

class SensitiveDetectorCatalog {
private:
	SDCatalog theCatalog;
	SDStore theStore;
	static SensitiveDetectorCatalog* thePointer;
	inline SensitiveDetectorCatalog();
//	SensitiveDetectorMessenger *theMessenger;
public:
	static SensitiveDetectorCatalog* GetSensitiveDetectorCatalog();
	void RegisterEntry(SensitiveDetectorEntry *d);
	void RegisterSensitiveDetector(FadsSensitiveDetector *d);
	SensitiveDetectorEntry *GetSensitiveDetectorEntry(std::string name);
	FadsSensitiveDetector *GetSensitiveDetector(std::string name);
	void PrintEntries();
	void PrintSensitiveDetectorList();
	SDStore* sdStore() { return &theStore; }
};

}	// end namespace

#endif	// SensitiveDetectorCatalog
