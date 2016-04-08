/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorConstructionEntryBase_H
#define DetectorConstructionEntryBase_H

#include <string>
#include "FadsControl/DetectorConstructionManager.h"
class G4VUserDetectorConstruction;

class DetectorConstructionEntryBase {
public:
	DetectorConstructionEntryBase(std::string n):name_(n) 
	{
		DetectorConstructionManager::AddDetectorConstructionEntry(this);
	}
	virtual ~DetectorConstructionEntryBase() {}
	std::string name() {return name_;}
	virtual void CreateEntry() =0;
	virtual void DeleteEntry() =0;
	virtual G4VUserDetectorConstruction* DetectorConstruction() = 0;
protected:
	std::string name_;
};

#endif
