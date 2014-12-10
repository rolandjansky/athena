/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef unionHandler_H
#define unionHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class unionHandler:public XMLHandler {
public:
	unionHandler(std::string);
	void ElementHandle();

};

#endif
