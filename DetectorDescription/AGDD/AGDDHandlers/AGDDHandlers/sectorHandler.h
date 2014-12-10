/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sectorHandler_H
#define sectorHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sectorHandler:public XMLHandler {
public:
	sectorHandler(std::string);
	void ElementHandle();

};

#endif
