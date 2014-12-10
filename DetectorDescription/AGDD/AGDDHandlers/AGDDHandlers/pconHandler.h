/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef pconHandler_H
#define pconHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class pconHandler:public XMLHandler {
public:
	pconHandler(std::string);
	void ElementHandle();

};

#endif
