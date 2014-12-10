/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef posRPhiZHandler_H
#define posRPhiZHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class AGDDPositioner;

class posRPhiZHandler:public XMLHandler {
public:
	posRPhiZHandler(std::string);
	void ElementHandle();

	AGDDPositioner *p;
};

#endif
