/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef varHandler_H
#define varHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class varHandler:public XMLHandler {
public:
	varHandler(std::string);
	void ElementHandle();

};

#endif
