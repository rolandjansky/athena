/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef colorHandler_H
#define colorHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDColor;

class colorHandler:public XMLHandler {
public:
	colorHandler(std::string);
	void ElementHandle();

	AGDDColor *color;
};

#endif
