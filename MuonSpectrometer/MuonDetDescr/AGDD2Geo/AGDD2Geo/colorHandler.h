/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef colorHandler_H
#define colorHandler_H

#include "AGDD2Geo/XMLHandler.h"
#include <string>

class colorHandler:public XMLHandler {
public:
	colorHandler(std::string);
	void ElementHandle();

};

#endif
