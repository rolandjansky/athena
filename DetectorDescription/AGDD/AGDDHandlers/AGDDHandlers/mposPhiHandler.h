/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mposPhiHandler_H
#define mposPhiHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class AGDDPositioner;

class mposPhiHandler:public XMLHandler {
public:
	mposPhiHandler(std::string);
	void ElementHandle();

	AGDDPositioner* p;
};

#endif
