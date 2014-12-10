/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef msgHandler_H
#define msgHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class msgHandler:public XMLHandler {
public:
	msgHandler(std::string);
	void ElementHandle();

};

#endif
