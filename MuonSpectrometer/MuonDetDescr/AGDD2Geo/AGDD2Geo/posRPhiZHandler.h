/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef posRPhiZHandler_H
#define posRPhiZHandler_H

#include "AGDD2Geo/XMLHandler.h"

#include <string>

class posRPhiZHandler:public XMLHandler {
public:
	posRPhiZHandler(std::string);
	void ElementHandle();

};

#endif
