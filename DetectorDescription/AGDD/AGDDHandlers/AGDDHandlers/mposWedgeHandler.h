/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mposWedgeHandler_H
#define mposWedgeHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDPositioner;

class mposWedgeHandler:public XMLHandler {
public:
	mposWedgeHandler(std::string);
	void ElementHandle();

	AGDDPositioner *p;
};

#endif
