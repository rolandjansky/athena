/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef arrayHandler_H
#define arrayHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class arrayHandler:public XMLHandler {
public:
	arrayHandler(std::string);
	void ElementHandle();

};

#endif
