/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGC_TechHandler_H
#define sTGC_TechHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sTGC_TechHandler:public XMLHandler {
public:
	sTGC_TechHandler(std::string);
	void ElementHandle();

};

#endif
