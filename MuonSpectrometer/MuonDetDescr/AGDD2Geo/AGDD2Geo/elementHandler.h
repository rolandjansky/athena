/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef elementHandler_H
#define elementHandler_H

#include "AGDD2Geo/XMLHandler.h"
#include <string>

class elementHandler:public XMLHandler {
public:
	elementHandler(std::string);
	void ElementHandle();

};

#endif
