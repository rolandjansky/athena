/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef consHandler_H
#define consHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class consHandler:public XMLHandler {
public:
	consHandler(std::string);
	void ElementHandle();

};

#endif
