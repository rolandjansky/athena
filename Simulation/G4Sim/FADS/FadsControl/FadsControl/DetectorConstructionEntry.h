/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorConstructionEntry_H
#define DetectorConstructionEntry_H

#include "FadsControl/DetectorConstructionEntryBase.h"
#include "G4VUserDetectorConstruction.hh"

template<class T> 
class DetectorConstructionEntry:public DetectorConstructionEntryBase {
public:
	DetectorConstructionEntry(std::string n):DetectorConstructionEntryBase(n),
				        theDetectorConstruction(0) {}
	void CreateEntry() {theDetectorConstruction=new T;}
	void DeleteEntry() {
		if (theDetectorConstruction) 
		{delete theDetectorConstruction;theDetectorConstruction=0;}
	}
	G4VUserDetectorConstruction* DetectorConstruction() {return theDetectorConstruction;} 
private:
	G4VUserDetectorConstruction* theDetectorConstruction;
};
#endif
