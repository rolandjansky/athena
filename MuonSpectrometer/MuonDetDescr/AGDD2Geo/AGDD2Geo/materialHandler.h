/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef materialHandler_H
#define materialHandler_H

#include "AGDD2Geo/XMLHandler.h"
#include <string>

class materialHandler:public XMLHandler {
public:
	materialHandler(std::string);
	void ElementHandle();

};

#endif
