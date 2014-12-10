/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef chamberPositionerHandler_H
#define chamberPositionerHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class chamberPositionerHandler:public XMLHandler {
public:
	chamberPositionerHandler(std::string);
	void ElementHandle();

};

#endif
