/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef addmaterialHandler_H
#define addmaterialHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class addmaterialHandler:public XMLHandler {
public:
	addmaterialHandler(std::string);
	void ElementHandle();
};

#endif
