/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ActionMenu_H
#define ActionMenu_H

#include <string>
#include "G4AtlasControl/SimpleMenu.h"

class ActionMenu : public SimpleMenu {
friend class SimControl;
private:
	ActionMenu();
public:
	void actionList() const;
	void actionDelete(const std::string) const;
	void actionRegister(const std::string , const std::string) const;
	void actionProperty(const std::string, const std::string, const std::string) const;
};

#endif
