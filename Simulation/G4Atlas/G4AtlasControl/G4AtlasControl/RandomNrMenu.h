/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RandomNrMenu_H
#define RandomNrMenu_H

#include <string>
#include "G4AtlasControl/SimpleMenu.h"

class RandomNrMenu : public SimpleMenu {
public:
	RandomNrMenu();
	void setSeed(long);
	void saveStatus(const std::string);
	void retrieveStatus(const std::string);
	void setEngine(const std::string);
	void resetEngine();
};

#endif
