/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SenDetectorMenu_H
#define SenDetectorMenu_H

#include "G4AtlasControl/SimpleMenu.h"

class SenDetectorMenu : public SimpleMenu {
public:
	SenDetectorMenu ();
	void printEntries();
	void printDetectors();
	void useSensitiveDetector(const std::string senDet, const std::string name,
				  bool allowMods=false);	
	void setAllowMods(const std::string senDet, bool allowMods=false);
};

#endif
