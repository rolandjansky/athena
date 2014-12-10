/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef snakeHandler_H
#define snakeHandler_H

#include "AGDDControl/XMLHandler.h"

#include <string>

class snakeHandler:public XMLHandler {
public:
	snakeHandler(std::string);
	void ElementHandle();

};

#endif
