/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SensitiveDetectorEntry_H
#define SensitiveDetectorEntry_H

#include <string>

namespace FADS {

class SensitiveDetectorEntry {
private:
	std::string name;
public:
	SensitiveDetectorEntry(std::string n);
	virtual ~SensitiveDetectorEntry() {}
	std::string GetName();
	virtual void Construct(std::string,bool) {;}
};

}	// end namespace

#endif	// SensitiveDetectorEntry
