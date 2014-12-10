/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef subtractionHandler_H
#define subtractionHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class subtractionHandler:public XMLHandler {
public:
	subtractionHandler(std::string);
	void ElementHandle();

};

#endif
