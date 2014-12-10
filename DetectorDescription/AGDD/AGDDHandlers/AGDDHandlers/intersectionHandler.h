/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef intersectionHandler_H
#define intersectionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class intersectionHandler:public XMLHandler {
public:
	intersectionHandler(std::string);
	void ElementHandle();

};

#endif
